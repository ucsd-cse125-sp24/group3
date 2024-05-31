#include "server/server.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <cassert>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <ostream>
#include <queue>
#include <thread>
#include <chrono>
#include <memory>

#include "boost/variant/get.hpp"
#include "server/game/objectmanager.hpp"
#include "server/game/potion.hpp"
#include "server/game/weapon.hpp"
#include "server/game/enemy.hpp"
#include "server/game/player.hpp"
#include "shared/game/event.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "shared/network/session.hpp"
#include "shared/network/packet.hpp"
#include "shared/network/constants.hpp"
#include "shared/utilities/config.hpp"
#include "shared/game/sharedobject.hpp"
#include "shared/utilities/constants.hpp"
#include "shared/utilities/light.hpp"
#include "shared/utilities/typedefs.hpp"
#include "shared/utilities/config.hpp"
#include "shared/utilities/rng.hpp"

using namespace std::chrono_literals;
using namespace boost::asio::ip;

Server::Server(boost::asio::io_context& io_context, GameConfig config)
    :lobby_broadcaster(io_context, config),
     acceptor(io_context, tcp::endpoint(tcp::v4(), config.network.server_port)),
     socket(io_context),
     world_eid(0),
     state(ServerGameState(GamePhase::LOBBY, config)),
     config(config)
{
    _doAccept(); // start asynchronously accepting

    if (config.server.lobby_broadcast) {
        this->lobby_broadcaster.startBroadcasting(ServerLobbyBroadcastPacket {
            .lobby_name  = config.server.lobby_name,
            .slots_taken = 0,
            .slots_avail = config.server.max_players});
    }
}

//  Note: This method should probably be removed since EntityIDs for objects
//  are assigned by the ObjectManager
EntityID Server::genNewEID() {
    static EntityID id = 1;
    return id++;
}

void Server::updateGameState(const EventList& events) {
    // TODO: remove cppcheck suppress when src_eid is being used

    // TODO : validate events if necessary
    this->state.update(events);
}

EventList Server::getAllClientEvents() {
    EventList allEvents;

    // Loop through each session
    for (const auto& [eid, _is_dm, _ip, session] : this->sessions) { // cppcheck-suppress unusedVariable
        if (session->isOkay()) {
            // Get events from the current session
            std::vector<Event> sessionEvents = session->handleAllReceivedPackets();

            // Put events into the allEvents vector, prepending each event with the id of the 
            // client that requested it
            std::transform(sessionEvents.begin(), sessionEvents.end(), std::back_inserter(allEvents), 
                [eid](const Event& e) {
                    return std::make_pair(eid, e);
                });
        }
    }

    return allEvents;
}

void Server::sendUpdateToAllClients(Event event) {
    for (const auto& [_eid, is_dm, _ip, session] : this->sessions) { // cppcheck-suppress unusedVariable
        if (session->isOkay()) {
            session->sendEvent(event);
        }
    }

}

void Server::sendLightSourceUpdates(EntityID playerID) {
    struct CompareLightPos {
        CompareLightPos() = default;
        CompareLightPos(const glm::vec3& refPos, ObjectManager& objects) : 
            refPos(refPos), objects(objects) {};
        bool operator()(const EntityID& a, const EntityID& b) const {
            glm::vec3 aPos = this->objects.getObject(a)->physics.shared.getCenterPosition();
            glm::vec3 bPos = this->objects.getObject(b)->physics.shared.getCenterPosition();

            float distanceToA = glm::distance(this->refPos, aPos);
            float distanceToB = glm::distance(this->refPos, bPos);
            return distanceToA > distanceToB;
        };
        glm::vec3 refPos;
        ObjectManager& objects;
    };

    glm::vec3 playerPos = this->state.objects.getObject(playerID)->physics.shared.getCenterPosition();

    std::priority_queue<EntityID, std::vector<EntityID>, CompareLightPos> closestPointLights(CompareLightPos(playerPos, this->state.objects));

    for(int i = 0; i < this->state.objects.getTorchlights().size(); i++) {
        auto torch = this->state.objects.getTorchlights().get(i);
        if (torch == nullptr) continue;
        closestPointLights.push(torch->globalID);
    }


    // put set into an array
    UpdateLightSourcesEvent event_data;
    int curr_light_num = 0;
    while (!closestPointLights.empty() && curr_light_num < MAX_POINT_LIGHTS) {
        EntityID light_id = closestPointLights.top(); 
        closestPointLights.pop();

        auto torchlight = dynamic_cast<Torchlight*>(this->state.objects.getObject(light_id));
        if (torchlight != nullptr) {
            event_data.lightSources[curr_light_num] = UpdateLightSourcesEvent::UpdatedLightSource {
                .eid = light_id,
                .intensity = torchlight->getIntensity()
            };
        } 
        curr_light_num++;
    }

    auto& by_id = this->sessions.get<IndexByID>();
    auto session_ref = by_id.find(playerID);
    if (session_ref != by_id.end()) {
        auto session = session_ref->session;
        if (session->isOkay()) {
            session->sendEvent(Event(
                this->world_eid,
                EventType::UpdateLightSources,
                event_data));
        }
    }
}

std::chrono::milliseconds Server::doTick() {
    auto start = std::chrono::high_resolution_clock::now();

    switch (this->state.getPhase()) {
        case GamePhase::LOBBY: {
            //  Go through sessions and update GameState lobby info
            for (const auto& [eid, is_dm, ip, session] : this->sessions) {
                if (session->isOkay()) {
                    this->state.addPlayerToLobby(LobbyPlayer(eid, PlayerRole::Unknown, false));
                }
                else {
                    this->state.removePlayerFromLobby(eid);
                }
            }

            if (this->state.getLobby().numPlayersInLobby() >= this->state.getLobby().max_players) {
                //  Selectively broadcast only to players that were already in the lobby?
                //  Note: This is currently marked as a TODO in the dungeon master branch
            }

            //  Handle ready and start game events
            EventList clientEvents = getAllClientEvents();

            for (const auto& [src_eid, event] : clientEvents) {
                //  Skip non-lobby action events
                if (event.type != EventType::LobbyAction) {
                    continue;
                }

                std::cout << "Received a LobbyAction event!" << std::endl;

                LobbyActionEvent lobbyEvent = boost::get<LobbyActionEvent>(event.data);

                switch (lobbyEvent.action) {
                case LobbyActionEvent::Action::Ready: {
                    //  Client player declares themselves ready
                    boost::optional<LobbyPlayer> player = this->state.getLobby().getPlayer(src_eid);

                    if (!player.has_value()) {
                        //  Client's src EntityID doesn't match any players
                        //  in the lobby! Crash the server
                        std::cerr << "Client's src eid doesn't match any players!" << std::endl;
                        std::exit(1);
                    }

                    if (lobbyEvent.role != PlayerRole::Unknown) {
                        //  Update player's role from lobby event if
                        //  player chose a role
                        player.get().desired_role = lobbyEvent.role;
                        player.get().ready = true;

                        this->state.updateLobbyPlayer(src_eid, player.get());
                    }
                    break;
                }
                case LobbyActionEvent::Action::StartGame: {
                    //  Client tries to start the game
                    //  Verify that all players are indeed ready
                    bool allReady = true;
                    for (boost::optional<LobbyPlayer> player : this->state.getLobby().players) {
                        if (!player.has_value() || !player.get().ready) {
                            //  Either there aren't enough players or at least
                            //  one player isn't ready
                            allReady = false;
                            break;
                        }
                    }

                    if (allReady) {
                        
                        if (!this->config.server.disable_dm) {
                            //  Randomly select a player from those whose desired role is
                            //  PlayerRole::DungeonMaster (or from all players if no player
                            //  has desired role set to PlayerRole::DungeonMaster) to be
                            //  the Dungeon Master. Replace that player's Player object in 
                            //  the ObjectManager to be the DungeonMaster

                            //  Determine list of players that want to play as the DM
                            std::vector<LobbyPlayer> wannabe_dms;

                            for (boost::optional<LobbyPlayer> player : this->state.getLobby().players) {
                                if (player.get().desired_role == PlayerRole::DungeonMaster) {
                                    wannabe_dms.push_back(player.get());
                                }
                            }

                            //  If no player wants to be a DM, then randomly choose one of them
                            if (wannabe_dms.size() == 0) {
                                for (boost::optional<LobbyPlayer> player : this->state.getLobby().players) {
                                    wannabe_dms.push_back(player.get()); // cppcheck-suppress useStlAlgorithm
                                }
                            }

                            //  Randomly select a DM
                            size_t randomPlayerIndex = randomInt(0, wannabe_dms.size() - 1);
                            LobbyPlayer new_dm = wannabe_dms[randomPlayerIndex];

                            this->state.objects.replaceObject(new_dm.id, new DungeonMaster(this->state.getGrid().getRandomSpawnPoint() + glm::vec3(0.0f, 25.0f, 0.0f), glm::vec3(0.0f)));
                            DungeonMaster* dm = this->state.objects.getDM();

                            //  Initialize DM's lightning bolt
                            SpecificID lightningID = this->state.objects.createObject(new Weapon(glm::vec3(-1.0f, 0, -1.0f), glm::vec3(0.0f), WeaponType::Lightning));
                            Weapon* lightning = dynamic_cast<Weapon*>(this->state.objects.getItem(lightningID));
                            lightning->iteminfo.held = true;
                            lightning->physics.collider = Collider::None;
                            dm->lightning = lightning;

                            auto& by_id = this->sessions.get<IndexByID>();
                            auto session_entry = by_id.find(dm->globalID);

                            if (session_entry != by_id.end()) {
                                std::weak_ptr<Session> session_ref = session_entry->session;
                                std::shared_ptr<Session> session = session_ref.lock();
                                if (session != nullptr) {
                                    session->sendPacket(PackagedPacket::make_shared(PacketType::ServerAssignEID,
                                        ServerAssignEIDPacket{ .eid = dm->globalID, .is_dungeon_master = true }));
                                }
                            }

                            //  Get DM's player index
                            int index = 1;
                            for (boost::optional<LobbyPlayer> player : this->state.getLobby().players) {
                                if (player.get().id == dm->globalID) {
                                    break;
                                }
                                index++;
                            }

                            this->state.markAsUpdated(dm->globalID);
                            for (const auto& partial_update : this->state.generateSharedGameState(false)) {
                                sendUpdateToAllClients(Event(this->world_eid, EventType::LoadGameState, LoadGameStateEvent(partial_update)));
                            }

                            std::cout << "Assigned player " + std::to_string(index) + " to be the DM" << std::endl;
                            std::cout << "Starting game!" << std::endl;
                        }

                        this->state.setPhase(GamePhase::INTRO_CUTSCENE);
                    }

                    break;
                }
                }
            }

            this->lobby_broadcaster.setLobbyInfo(this->state.getLobby());

            //std::cout << this->state.getLobby().to_string() << std::endl;

            break;
        }
        case GamePhase::INTRO_CUTSCENE: {
            bool finished = this->intro_cutscene.update();
            if (finished) {
                this->state.setPhase(GamePhase::GAME);
            } else {
                LoadIntroCutsceneEvent update = this->intro_cutscene.toNetwork();
                sendUpdateToAllClients(Event(this->world_eid, EventType::LoadIntroCutscene, update));
            }

            break;
        }

        case GamePhase::GAME: {
            EventList allClientEvents = getAllClientEvents();

            updateGameState(allClientEvents);

            static int curr_player_idx = 0;

            EntityID player_id = this->state.getLobby().players.at(curr_player_idx).get().id;

            sendLightSourceUpdates(player_id);
            curr_player_idx = (curr_player_idx + 1) % this->state.getLobby().max_players;

            break;
        }
        case GamePhase::RESULTS: {
            //  Do nothing - in this phase, the client(s) just display the
            //  end-of-match data to the players
            break;
        }

        default:
            std::cerr << "Invalid GamePhase on server:" << static_cast<int>(this->state.getPhase()) << std::endl;
            std::exit(1);
    }

    this->sendSoundCommands();

    // send partial updates to the clients
    // ALSO where the packets actually get sent
    for (const auto& partial_update: this->state.generateSharedGameState(false)) {
        sendUpdateToAllClients(Event(this->world_eid, EventType::LoadGameState, LoadGameStateEvent(partial_update)));
    }

    // Calculate how long we need to wait until the next tick
    auto stop = std::chrono::high_resolution_clock::now();
    auto wait = std::chrono::duration_cast<std::chrono::milliseconds>(
        TIMESTEP_LEN - (stop - start));
    return wait;
}

void Server::_doAccept() {
    this->acceptor.async_accept(this->socket,
        [this](boost::system::error_code ec) {
            if (!ec) {
                auto addr = this->socket.remote_endpoint().address();
                auto new_session = this->_handleNewSession(addr);

                // send complete gamestate to the new person who connected
                int i = 0;
                for (auto& partial_update : this->state.generateSharedGameState(true)) {
                    new_session->sendEvent(Event(0, EventType::LoadGameState, LoadGameStateEvent(partial_update)));
                }

                new_session->sendPacket(PackagedPacket::make_shared(PacketType::ServerAssignEID,
                    ServerAssignEIDPacket { .eid = new_session->getInfo().client_eid.value(), 
                                            .is_dungeon_master = new_session->getInfo().is_dungeon_master.value()}));
            } else {
                std::cerr << "Error accepting tcp connection: " << ec << std::endl;

                if (ec == boost::asio::error::already_open) {
                    
                }
            }

            this->_doAccept();
        });
}

std::shared_ptr<Session> Server::_handleNewSession(boost::asio::ip::address addr) {
    auto& by_ip = this->sessions.get<IndexByIP>();
    auto old_session = by_ip.find(addr);

    if (old_session != by_ip.end()) {
        // We already had a session with this IP
        if (!old_session->session->isOkay()) {
            EntityID old_id = old_session->id;

            // The old session is dead, so create new one

            auto new_session = std::make_shared<Session>(std::move(this->socket),
                SessionInfo({}, old_id, old_session->is_dungeon_master));
            by_ip.replace(old_session, SessionEntry(old_id, old_session->is_dungeon_master, addr, new_session));

            std::cout << "Reestablished connection with " << addr 
                << ", which was previously assigned eid " << old_id << std::endl;
            
            return new_session;

        } else {
            // Some some reason the session is still alive, but we are getting
            // a connection request from the host?
            std::cerr << "Error: incoming connection request from " << addr
                << " with which we already have an active session" << std::endl;

            return old_session->session;
        }
    }

    // Brand new connection
    // TODO: reject connection if not in LOBBY GamePhase
    Player* player = new Player(this->state.getGrid().getRandomSpawnPoint(), glm::vec3(0.0f));
    this->state.objects.createObject(player);

    auto session = std::make_shared<Session>(std::move(this->socket),
        SessionInfo({}, player->globalID, false));

    this->sessions.insert(SessionEntry(player->globalID, false, addr, session));

    std::cout << "Established new connection with " << addr << ", which was assigned eid "
        << player->globalID << std::endl;

    return session;
}

void Server::sendSoundCommands() {
    bool is_intro_cutscene = this->state.getPhase() == GamePhase::INTRO_CUTSCENE;    

    ServerGameState& curr_state = (is_intro_cutscene) ? this->intro_cutscene.state : this->state;

    // TODO: send sound effects to DM?
    std::vector<Object*> players; // hold players and DM
    for (int i = 0; i < curr_state.objects.getPlayers().size(); i++) {
        auto player = curr_state.objects.getPlayer(i);
        if (player != nullptr) {
            players.push_back(player);
        }
    }
    if (curr_state.objects.getDM() != nullptr) {
        players.push_back(curr_state.objects.getDM());
    }

    auto audio_commands_per_player = curr_state.soundTable().getCommandsPerPlayer(players);

    for (auto& session_entry : this->sessions) {
        EntityID eid = (is_intro_cutscene) ? this->intro_cutscene.pov_eid : session_entry.id;

        if (!audio_commands_per_player.contains(eid)) {
            continue; // no sounds to send to that player
        }

        auto session = session_entry.session;
        if (!session->isOkay()) {
            continue; // lost connection with this session, so can't send audio updates to it
        }

        session->sendEvent(Event(this->world_eid, EventType::LoadSoundCommands, LoadSoundCommandsEvent(
            audio_commands_per_player.at(eid)
        )));
    }

    curr_state.soundTable().tickSounds();
}