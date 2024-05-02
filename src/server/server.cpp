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
    state.objects.createObject(ObjectType::Object);

    ////  Create a room
    //EntityID wall1ID = state.objects.createObject(ObjectType::SolidSurface);
    //EntityID wall2ID = state.objects.createObject(ObjectType::SolidSurface);
    //EntityID wall3ID = state.objects.createObject(ObjectType::SolidSurface);
    //EntityID wall4ID = state.objects.createObject(ObjectType::SolidSurface);
    //EntityID floorID = state.objects.createObject(ObjectType::SolidSurface);

    ////  Specify wall positions
    ////  Configuration: 40 (x) x 32 (y) room example
    ////  ##1##
    ////  #   #
    ////  2   3
    ////  #   #
    ////  ##4##

    //SolidSurface* wall1 = (SolidSurface*)state.objects.getObject(wall1ID);
    //SolidSurface* wall2 = (SolidSurface*)state.objects.getObject(wall2ID);
    //SolidSurface* wall3 = (SolidSurface*)state.objects.getObject(wall3ID);
    //SolidSurface* wall4 = (SolidSurface*)state.objects.getObject(wall4ID);
    //SolidSurface* floor = (SolidSurface*)state.objects.getObject(floorID);

    ////  Wall1 has dimensions (40, 1, 4) and position (0, 15.5, 2)
    //wall1->shared.dimensions = glm::vec3(40, 1, 4);
    //wall1->physics.shared.position = glm::vec3(0, 15.5, 2);
    //wall1->physics.movable = false;

    ////  Wall2 has dimensions (30, 1, 4) and position (-19.5, 0, 2)
    //wall2->shared.dimensions = glm::vec3(30, 1, 4);
    //wall2->physics.shared.position = glm::vec3(-19.5, 0, 2);
    //wall2->physics.movable = false;

    ////  Wall3 has dimensions (30, 1, 4) and position (19.5, 0, 2)
    //wall3->shared.dimensions = glm::vec3(30, 1, 4);
    //wall3->physics.shared.position = glm::vec3(19.5, 0, 2);
    //wall3->physics.movable = false;

    ////  Wall4 has dimensions (40, 1, 4) and position (0, -15.5, 2)
    //wall4->shared.dimensions = glm::vec3(40, 1, 4);
    //wall4->physics.shared.position = glm::vec3(0, -15.5, 2);
    //wall4->physics.movable = false;

    ////  floor has dimensions (40, 32, 1) and position (0, 0, -0.5)
    //floor->shared.dimensions = glm::vec3(40, 32, 1);
    //floor->physics.shared.position = glm::vec3(0, 0, -0.5);
    //floor->physics.movable = false;

    
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
    for (const auto& [eid, _ip, session] : this->sessions) { // cppcheck-suppress unusedVariable
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
    for (const auto& [_eid, _ip, session] : this->sessions) { // cppcheck-suppress unusedVariable
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
            for (const auto& [eid, ip, session]: this->sessions) {
                if (auto s = session.lock()) {
                    this->state.addPlayerToLobby(eid, s->getInfo().client_name.value_or("UNKNOWN NAME"));
                } else {
                    this->state.removePlayerFromLobby(eid);
                }
            }

            if (this->state.getLobbyPlayers().size() >= this->state.getLobbyMaxPlayers()) {
                this->state.setPhase(GamePhase::GAME);
            } else {
                std::cout << "Only have " << this->state.getLobbyPlayers().size() << "/" << this->state.getLobbyMaxPlayers() << "\n";
            }

            sendUpdateToAllClients(Event(this->world_eid, EventType::LoadGameState, LoadGameStateEvent(this->state.generateSharedGameState())));
            // Tell each client the current lobby status

            std::cout << "waiting for " << this->state.getLobbyMaxPlayers() << " players" << std::endl;

            break;
        case GamePhase::GAME: {
            EventList allClientEvents = getAllClientEvents();

            updateGameState(allClientEvents);

            sendUpdateToAllClients(Event(this->world_eid, EventType::LoadGameState, LoadGameStateEvent(this->state.generateSharedGameState())));
            break;
        }
        default:
            std::cerr << "Invalid GamePhase on server:" << static_cast<int>(this->state.getPhase()) << std::endl;
            std::exit(1);
    }

    // Calculate how long we need to wait until the next tick
    auto stop = std::chrono::high_resolution_clock::now();
    auto wait = std::chrono::duration_cast<std::chrono::milliseconds>(
        this->state.getTimestepLength() - (stop - start));
    assert(wait.count() > 0);
    return wait;
}

void Server::_doAccept() {
    this->acceptor.async_accept(this->socket,
        [this](boost::system::error_code ec) {
            if (!ec) {
                auto addr = this->socket.remote_endpoint().address();
                auto new_session = this->_handleNewSession(addr);

                new_session->startListen();
                std::cout << "about to send server assign id packet" <<std::endl;
                new_session->sendPacketAsync(PackagedPacket::make_shared(PacketType::ServerAssignEID,
                    ServerAssignEIDPacket { .eid = new_session->getInfo().client_eid.value() }));
            } else {
                std::cerr << "Error accepting tcp connection: " << ec << std::endl;
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
            auto new_session = std::make_shared<Session>(std::move(this->socket),
                SessionInfo({}, old_id));
            by_ip.replace(old_session, SessionEntry(old_id, addr, new_session));

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

    // Brand new connection
    // TODO: reject connection if not in LOBBY GamePhase
    EntityID id = this->state.objects.createObject(ObjectType::Player);
    auto session = std::make_shared<Session>(std::move(this->socket),
        SessionInfo({}, id));

    this->sessions.insert(SessionEntry(id, addr, session));

    std::cout << "Established new connection with " << addr << ", which was assigned eid "
        << id << std::endl;

    return session;
}