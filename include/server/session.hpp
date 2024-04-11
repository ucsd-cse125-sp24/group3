#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <mutex>

#include "shared/network/packet.hpp"

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session>
{
public:
    Session(tcp::socket socket, EntityID eid);
    ~Session();

    void do_write(PacketBuffer buf);

    std::vector<std::pair<packet::Type, std::string>> do_read();

private:
    tcp::socket socket;
    enum { max_length = 1024 };
    char data[max_length];

    /// @brief eid of the player that this session is associated with.
    EntityID eid;

    std::mutex mut;
    std::queue<std::pair<packet::Type, std::string>> incoming_packets;

    void do_read_background();
};