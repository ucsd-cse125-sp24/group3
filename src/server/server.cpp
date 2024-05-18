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
#include <thread>
#include <chrono>

#include "boost/variant/get.hpp"
#include "server/game/potion.hpp"
#include "server/game/enemy.hpp"
#include "server/game/player.hpp"
#include "shared/game/event.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/object.hpp"
#include "shared/network/session.hpp"
#include "shared/network/packet.hpp"
#include "shared/network/constants.hpp"
#include "shared/utilities/config.hpp"

using namespace std::chrono_literals;
using namespace boost::asio::ip;

Server::Server(boost::asio::io_context& io_context, GameConfig config)
    :lobby_broadcaster(io_context, config),
     acceptor(io_context, tcp::endpoint(tcp::v4(), config.network.server_port)),
     socket(io_context),
     world_eid(0),
     state(ServerGameState(GamePhase::LOBBY, config))
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
    for (const auto& [eid, is_dm, _ip, session] : this->sessions) { // cppcheck-suppress unusedVariable
        if (auto s = session.lock()) {
            // Get events from the current session
            std::vector<Event> sessionEvents = s->getEvents();

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
        if (auto s = session.lock()) {
            s->sendEventAsync(event);
        }
    }
}

std::chrono::milliseconds Server::doTick() {
    auto start = std::chrono::high_resolution_clock::now();

    switch (this->state.getPhase()) {
        case GamePhase::LOBBY:
            // Go through sessions and update GameState lobby info
            // TODO: move this into updateGameState or something else
            for (const auto& [eid, is_dm, ip, session]: this->sessions) {
                if (auto s = session.lock()) {
                    this->state.addPlayerToLobby(eid, s->getInfo().client_name.value_or("UNKNOWN NAME"));
                } else {
                    this->state.removePlayerFromLobby(eid);
                }
            }

            if (this->state.getLobby().players.size() >= this->state.getLobby().max_players) {
                this->state.setPhase(GamePhase::GAME);
                // TODO: figure out how to selectively broadcast to only the players that were already in the lobby
                // this->lobby_broadcaster.stopBroadcasting();
            } else {
                std::cout << "Only have " << this->state.getLobby().players.size()
                    << "/" << this->state.getLobby().max_players << "\n";
            }

            this->lobby_broadcaster.setLobbyInfo(this->state.getLobby());
            break;
        case GamePhase::GAME: {
            EventList allClientEvents = getAllClientEvents();

            updateGameState(allClientEvents);
            break;
        }
        default:
            std::cerr << "Invalid GamePhase on server:" << static_cast<int>(this->state.getPhase()) << std::endl;
            std::exit(1);
    }

    // send partial updates to the clients
    for (const auto& partial_update: this->state.generateSharedGameState(false)) {
        sendUpdateToAllClients(Event(this->world_eid, EventType::LoadGameState, LoadGameStateEvent(partial_update)));
    }

    // Calculate how long we need to wait until the next tick
    auto stop = std::chrono::high_resolution_clock::now();
    auto wait = std::chrono::duration_cast<std::chrono::milliseconds>(
        this->state.getTimestepLength() - (stop - start));
    return wait;
}

void Server::_doAccept() {
    this->acceptor.async_accept(this->socket,
        [this](boost::system::error_code ec) {
            if (!ec) {
                auto addr = this->socket.remote_endpoint().address();
                auto new_session = this->_handleNewSession(addr);

                // send complete gamestate to the new person who connected
                for (auto& partial_update : this->state.generateSharedGameState(true)) {
                    new_session->sendEventAsync(Event(0, EventType::LoadGameState, LoadGameStateEvent(partial_update)));
                }

                new_session->startListen();
                std::cout << "about to send server assign id packet" <<std::endl;
                new_session->sendPacketAsync(PackagedPacket::make_shared(PacketType::ServerAssignEID,
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
        if (old_session->session.expired()) {
            EntityID old_id = old_session->id;

            // The old session is expired, so create new one

            // this means if Dungeon Master drops, THE PLAYER WILL NOT BE ON RECONNECT!

            // TODO: need to change probably?
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

            return old_session->session.lock();
        }
    }

    // first player is Dungeon Master
    if (state.is_dungeon_master) {
        this->state.objects.createObject(new DungeonMaster(this->state.getGrid().getRandomSpawnPoint() + glm::vec3(0.0f, 25.0f, 0.0f), glm::vec3(0.0f)));
        DungeonMaster* dm = this->state.objects.getDM();

        //  Spawn player in random spawn point

        //  TODO: Possibly replace this random spawn point with player assignments?
        //  I.e., assign each player a spawn point to avoid multiple players getting
        //  the same spawn point?

        auto session = std::make_shared<Session>(std::move(this->socket),
            SessionInfo({}, dm->globalID, state.is_dungeon_master));


        this->sessions.insert(SessionEntry(dm->globalID, state.is_dungeon_master, addr, session));

        std::cout << "Established new connection with " << addr << ", which was assigned eid "
            << dm->globalID << std::endl;

        return session;
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