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
     state(GameState(GamePhase::GAME, config))
{
    Object* obj = state.createObject();
    obj->position = glm::vec3(0.0f, 0.0f, 0.0f);
    
    doAccept(); // start asynchronously accepting

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

std::chrono::milliseconds Server::doTick() {
    auto start = std::chrono::high_resolution_clock::now();

    switch (this->state.getPhase()) {
        case GamePhase::LOBBY:

            // Go through sessions and update GameState lobby info
            for (const auto& [eid, ip, session]: this->sessions) {
                if (auto s = session.lock()) {
                    this->state.addPlayerToLobby(eid, s->getInfo().client_name.value_or("UNKNOWN NAME"));
                } else {
                    this->state.removePlayerFromLobby(eid);
                }
            }

            // Tell each client the current lobby status
            for (const auto& [eid, ip, session]: this->sessions) {
                if (auto s = session.lock()) {
                    s->sendEventAsync(Event(this->world_eid,
                        EventType::LoadGameState, LoadGameStateEvent(this->state)));
                }
            };

            break;
        case GamePhase::GAME:
            for(const auto& [eid, session]: this->sessions) {
                session->sendEventAsync(Event(this->world_eid, EventType::LoadGameState, LoadGameStateEvent(this->state)));
                std::vector<Event> events = session->getEvents();
                for(const Event& event: events) {
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
            break;
        default:
            std::cerr << "Non Lobby State not implemented on server side yet" << std::endl;
            std::exit(1);
    }


    auto stop = std::chrono::high_resolution_clock::now();
    auto wait = std::chrono::duration_cast<std::chrono::milliseconds>(
        this->state.getTimestepLength() - (stop - start));
    assert(wait.count() > 0);
    return wait;
}

void Server::doAccept() {
    this->acceptor.async_accept(this->socket,
        [this](boost::system::error_code ec) {
            if (!ec) {
                EntityID eid = Server::genNewEID();
                auto session = std::make_shared<Session>(std::move(this->socket),
                    SessionInfo({}, eid));

                this->sessions.insert(SessionEntry(eid, 
                    this->socket.remote_endpoint().address(), session));

                session->startListen();

                session->sendPacketAsync(PackagedPacket::make_shared(PacketType::ServerAssignEID,
                    ServerAssignEIDPacket { .eid = eid }));
            } else {
                std::cerr << "Error accepting tcp connection: " << ec << std::endl;
            }

            doAccept();
        });
}
