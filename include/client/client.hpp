#pragma once

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_service.hpp>

#include <utility>
#include <unordered_map>

#include "shared/network/packet.hpp"

using namespace boost::asio::ip;

class Client {
public:
    Client(boost::asio::io_service& io_service);

private:
    udp::socket lobby_discovery_socket;
    std::array<char, 512> lobby_info_buf;
    udp::endpoint endpoint_buf;
    std::unordered_map<udp::endpoint, packet::ServerLobbyBroadcast> lobbies_avail;
};

