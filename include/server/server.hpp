#pragma once

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>

#include <thread>
#include <atomic>
#include <unordered_map>
#include <memory>
#include <chrono>

#include "server/lobbybroadcaster.hpp"
#include "shared/network/session.hpp"
#include "shared/utilities/config.hpp"
#include "shared/utilities/typedefs.hpp"

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_context& io_context, GameConfig config);
    void doAccept();

    static EntityID genNewEID();

    /**
     * Do one game tick
     * 
     * @returns Time in MS to wait until starting next tick
     */
    std::chrono::milliseconds doTick();

    std::vector<Event> getAllClientEvents();

    void updateGameState(const std::vector<Event>& events);

    void sendUpdateToAllClients(Event event);

private:
    /// @brief EID that is reserved for the Server / World itself.
    EntityID world_eid;

    /// @brief Broadcaster which spawns up another thread advertising this lobby
    LobbyBroadcaster lobby_broadcaster;

    tcp::acceptor acceptor;
    tcp::socket socket;

    /// @brief Mapping from player id to session for that player.
    std::unordered_map<EntityID, std::shared_ptr<Session>> sessions;

    /// @brief Master copy of the GameState, living on the server
    GameState state;
};
