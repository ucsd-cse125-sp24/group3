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

private:
    EntityID world_eid;

    LobbyBroadcaster lobby_broadcaster;

    tcp::acceptor acceptor;
    tcp::socket socket;

    std::unordered_map<EntityID, std::shared_ptr<Session>> sessions;

    GameState state;
};
