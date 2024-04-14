#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>

#include <utility>
#include <unordered_map>

#include "shared/network/packet.hpp"
#include "client/lobbyfinder.hpp"
#include "client/clientinfo.hpp"
#include "shared/network/session.hpp"

using namespace boost::asio::ip;

class Client {
public:
    Client(boost::asio::io_service& io_service, std::string ip_addr);

    void connectAndListen();

//private:
    tcp::resolver resolver;
    tcp::socket socket;
    basic_resolver_results<class boost::asio::ip::tcp> endpoints;
    // ClientInfo info;
    std::shared_ptr<Session> client_session;
};

