#pragma once

#include <boost/asio/ip/tcp.hpp>

#include <string>
#include <memory>

#include "shared/network/packet.hpp"

using namespace boost::asio::ip;

using Packets = std::vector<std::pair<packet::Type, std::string>>;

class GameSocket {
public:
    GameSocket(std::shared_ptr<tcp::socket> socket, int identifier);
    GameSocket(GameSocket& other);

    void send(std::shared_ptr<PackagedPacket> packet);

    Packets receive();

private:
    int identifier;

    std::shared_ptr<tcp::socket> socket;
    enum { max_length = 4096 }; // might need to increase this depending on size of game state serialization
    char data[max_length];

    std::mutex mut; // might be able to remove this
    Packets incoming_packets;

    void _receiveHdr();
    void _receiveData(packet::Header hdr);

    enum error_type {
        FATAL,
        RETRY,
        OKAY
    };

    error_type _classifyError(boost::system::error_code ec, const char* where);
    error_type last_error;
};
