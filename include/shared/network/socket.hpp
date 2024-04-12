#pragma once

#include <boost/asio/ip/tcp.hpp>

#include <memory>

#include "shared/network/packet.hpp"

using namespace boost::asio::ip;

// TODO: Javadocs
void sendPacketAsync(tcp::socket& socket, std::shared_ptr<PackagedPacket> packet);
void receivePacketAsync(tcp::socket& socket, std::shared_ptr<PackagedPacket> packet);

enum SocketError {
    NONE,
    FATAL,
    RETRY,
};

SocketError _classifySocketError(boost::system::error_code ec, const char* where);