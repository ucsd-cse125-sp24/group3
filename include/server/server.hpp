#pragma once

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>

#include <thread>
#include <atomic>

#include "server/lobbybroadcaster.hpp"

class Server {
public:
    Server(boost::asio::io_context& io_context);

private:
    LobbyBroadcaster lobby_broadcaster;
};
