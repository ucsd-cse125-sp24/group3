#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>

#include <utility>
#include <unordered_map>

#include "shared/network/packet.hpp"
#include "client/lobbyfinder.hpp"

using namespace boost::asio::ip;

class Client {
public:
    Client(boost::asio::io_service& io_service);

    void connect(tcp::endpoint server);

private:

};

