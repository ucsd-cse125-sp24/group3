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

void Server::updateGameState(const std::vector<Event>& events) {
    for (const Event& event : events) {
        switch (event.type) {
        case EventType::MoveRelative:
            auto moveRelativeEvent = boost::get<MoveRelativeEvent>(event.data);
            Object* obj = state.objects.getObject(moveRelativeEvent.entity_to_move);
            //obj->setPosition(obj->position + moveRelativeEvent.movement);
            obj->physics.shared.position += moveRelativeEvent.movement;
            break;
            // default:
            //     std::cerr << "Unimplemented EventType (" << event.type << ") received" << std::endl;
        }
    }
}

std::vector<Event> Server::getAllClientEvents() {
    std::vector<Event> allEvents;

    // Loop through each session
    for (const auto& [eid, session] : this->sessions) { // cppcheck-suppress unusedVariable
        // Get events from the current session
        std::vector<Event> sessionEvents = session->getEvents();

        // Append session events to the overall vector
        allEvents.insert(allEvents.end(), sessionEvents.begin(), sessionEvents.end());
    }

    return allEvents;
}

void Server::sendUpdateToAllClients(Event event) {
    for (const auto& [eid, session] : this->sessions) { // cppcheck-suppress unusedVariable
        session->sendEventAsync(event); // SEND UPDATED GAME STATE TO CLIENTS
    }
}

std::chrono::milliseconds Server::doTick() {
    auto start = std::chrono::high_resolution_clock::now();

    switch (this->state.getPhase()) {
        case GamePhase::LOBBY:
            // Go through sessions and update GameState lobby info
            // Right now just always resetting and then readding so we make sure
            // we have the up-to-date info
            // TODO: logic to determine if a session is dropped using std::weak_ptr
            //       and then call state.removePlayerFromLobby if dropped.
            for (const auto& [eid, session]: this->sessions) {
                this->state.addPlayerToLobby(eid,
                    session->getInfo().client_name.value_or("[UNKNOWN NAME]"));
            }

            if (this->state.getLobbyPlayers().size() >= this->state.getLobbyMaxPlayers()) {
                this->state.setPhase(GamePhase::GAME);
            }

            // Tell each client the current lobby status
            for (const auto& [eid, session]: this->sessions) { // cppcheck-suppress unusedVariable
                session->sendEventAsync(Event(this->world_eid,
                    EventType::LoadGameState, LoadGameStateEvent(this->state.generateSharedGameState())));
            };

            std::cout << "waiting for " << this->state.getLobbyMaxPlayers() << " players" << std::endl;

            break;
        case GamePhase::GAME: {
            std::vector<Event> allClientEvents = getAllClientEvents();

            updateGameState(allClientEvents);

            sendUpdateToAllClients(Event(this->world_eid, EventType::LoadGameState, LoadGameStateEvent(this->state.generateSharedGameState())));
            break;
        }
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
                auto session = std::make_shared<Session>(std::move(this->socket), SessionInfo {
                    .client_name = {},
                    .client_eid = eid
                });

                session->startListen();

                this->sessions.insert({eid, session});

                session->sendPacketAsync(PackagedPacket::make_shared(PacketType::ServerAssignEID,
                    ServerAssignEIDPacket { .eid = eid }));
            } else {
                std::cerr << "Error accepting tcp connection: " << ec << std::endl;
            }

            doAccept();
        });
}
