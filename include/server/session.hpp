#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>

#include "shared/network/packet.hpp"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket, EntityID eid);
    ~Session();

    void send(PacketBuffer buf);

    std::vector<std::pair<packet::Type, std::string>> receive();

private:
    tcp::socket socket;
    enum { max_length = 2048 }; // might need to increase this depending on size of game state serialization
    char data[max_length];

    /// @brief eid of the player that this session is associated with.
    EntityID eid;

    std::mutex mut;
    std::queue<std::pair<packet::Type, std::string>> incoming_packets;

    void _receiveHdr();
    void _receiveData(packet::Header hdr);
};