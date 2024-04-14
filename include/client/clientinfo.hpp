#pragma once

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_service.hpp>

#include <utility>

using namespace boost::asio::ip;

class ClientInfo {
public:
    ClientInfo();

    std::string getName();

    EntityID getEID();

private:
    std::string player_name;
    EntityID eid;
};

