#pragma once

#include <boost/asio/ip/tcp.hpp>

#include <string>

#include "shared/network/packet.hpp"

using namespace boost::asio::ip;

using Packets = std::vector<std::pair<packet::Type, std::string>>;

/**
 * Class which wraps around a tcp socket and manages writing/reading packets
 * with header information. I don't know if this class is safely managing error
 * cases right now. I forsee some debugging to make it actually handle errors
 * correctly.
 */
class GameSocket {
public:
    GameSocket(tcp::socket&& socket, int identifier);
    GameSocket(GameSocket&& other);
    ~GameSocket();

    void send(PacketBuffer buf);

    Packets receive();

private:
    int identifier;

    tcp::socket socket;
    enum { max_length = 4096 }; // might need to increase this depending on size of game state serialization
    char data[max_length];

    std::mutex mut;
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
