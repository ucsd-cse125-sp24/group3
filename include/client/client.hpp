#pragma once

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_service.hpp>

using namespace boost::asio::ip;

class Client {
public:
    Client(boost::asio::io_service& io_service);

private:
    udp::socket lobby_discovery_socket; 
};

