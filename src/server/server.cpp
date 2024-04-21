#include "server/server.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <cassert>

#include <iostream>
#include <fstream>
#include <ostream>
#include <thread>
#include <chrono>

#include "boost/variant/get.hpp"
#include "shared/game/event.hpp"
#include "shared/game/gamelogic/object.hpp"
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
     state(GameState(GamePhase::LOBBY, config))
{
    Object* obj = state.createObject();
    obj->position = glm::vec3(0.0f, 0.0f, 0.0f);
    
    _doAccept(); // start asynchronously accepting

    if (config.server.lobby_broadcast) {
        this->lobby_broadcaster.startBroadcasting(ServerLobbyBroadcastPacket {
            .lobby_name  = config.server.lobby_name,
            .slots_taken = 0,
            .slots_avail = config.server.max_players});
    }
}

EntityID Server::genNewEID() {
    static EntityID id = 1;
    return id++;
}

void Server::updateGameState(std::vector<Event> events) {
    for (const Event& event : events) {
        switch (event.type) {
        case EventType::MoveRelative:
            auto moveRelativeEvent = boost::get<MoveRelativeEvent>(event.data);
            Object* obj = state.getObject(moveRelativeEvent.entity_to_move);
            obj->setPosition(obj->position + moveRelativeEvent.movement);
            break;
            // default:
            //     std::cerr << "Unimplemented EventType (" << event.type << ") received" << std::endl;
        }
    }
}

std::vector<Event> Server::getAllClientEvents() {
    std::vector<Event> allEvents;

    // Loop through each session
    for (const auto& [_eid, _ip, session] : this->sessions) {
        if (auto s = session.lock()) {
            // Get events from the current session
            std::vector<Event> sessionEvents = s->getEvents();

            // Append session events to the overall vector
            allEvents.insert(allEvents.end(), sessionEvents.begin(), sessionEvents.end());
        }
    }

    return allEvents;
}

void Server::sendUpdateToAllClients(Event event) {
    for (const auto& [_eid, _ip, session] : this->sessions) {
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

            if (this->state.enoughPlayers()) {
                this->state.setPhase(GamePhase::GAME);
            }

            sendUpdateToAllClients(Event(this->world_eid, EventType::LoadGameState, LoadGameStateEvent(this->state)));

            std::cout << "not enough players to start\n";

            break;
        case GamePhase::GAME: {
            std::vector<Event> allClientEvents = getAllClientEvents();

            updateGameState(allClientEvents);

            sendUpdateToAllClients(Event(this->world_eid, EventType::LoadGameState, LoadGameStateEvent(this->state)));
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
    EntityID id = Server::genNewEID();
    auto session = std::make_shared<Session>(std::move(this->socket),
        SessionInfo({}, id));

    this->sessions.insert(SessionEntry(id, addr, session));

    std::cout << "Established new connection with " << addr << ", which was assigned eid "
        << id << std::endl;

    session->startListen();
    session->sendPacketAsync(PackagedPacket::make_shared(PacketType::ServerAssignEID,
        ServerAssignEIDPacket { .eid = id }));

    return session;
}