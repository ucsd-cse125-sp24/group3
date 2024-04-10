#pragma once

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>

#include <thread>
#include <atomic>

class Server {
public:
    Server(boost::asio::io_context& io_context);

private:
    boost::asio::ip::udp::socket lobby_broadcast_socket;
    std::thread lobby_broadcast_thread;
    std::atomic_bool keep_broadcasting_lobby;

    void _broadcastLobbyWorker();
};
