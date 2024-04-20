#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>

#include <utility>
#include <unordered_map>

#include "shared/network/packet.hpp"
#include "client/lobbyfinder.hpp"
#include "shared/network/session.hpp"
#include "shared/utilities/config.hpp"

using namespace boost::asio::ip;

class Client {
public:
    Client(boost::asio::io_service& io_service, GameConfig config);

    /**
     * Client connects to the IP address and listen for packets
     */
    void connectAndListen(std::string ip_addr);

    std::shared_ptr<Session> session;

private:
    GameConfig config;
    tcp::resolver resolver;
    tcp::socket socket;

    /// @brief Generate endpoints the client can connect to
    basic_resolver_results<class boost::asio::ip::tcp> endpoints;
};

