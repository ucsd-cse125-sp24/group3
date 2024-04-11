#pragma once

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>

#include <thread>
#include <atomic>

#include "server/lobbybroadcaster.hpp"

using boost::asio::ip::tcp;

class Server {
public:
    Server(boost::asio::io_context& io_context);
    void do_accept();

private:
    LobbyBroadcaster lobby_broadcaster;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
};
