#pragma once

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>

#include <thread>
#include <atomic>
#include <unordered_map>
#include <memory>

#include "server/lobbybroadcaster.hpp"
#include "shared/network/session.hpp"

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_context& io_context);
    void doAccept();

    static EntityID genNewEID();

private:
    LobbyBroadcaster lobby_broadcaster;

    tcp::acceptor acceptor;
    tcp::socket socket;

    std::unordered_map<EntityID, std::shared_ptr<Session>> sessions;
};
