#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include <string>

#include "shared/network/packet.hpp"

using boost::asio::ip::tcp;

enum SocketError {
    NONE,
    FATAL,
    RETRY,
};

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket, EntityID eid);
    ~Session();

    void startListen();

    void addReceivedPacket(packet::Type type, std::string data);

    std::vector<std::pair<packet::Type, std::string>> getAllReceivedPackets();

    void sendPacketAsync(std::shared_ptr<PackagedPacket> packet);

    void receivePacketAsync();

private:
    tcp::socket socket;

    // TODO: determine if we need a mutex to proect this or not,
    // might not because we will only be checking received packets
    // during the game tick, and will only be running background async 
    // tasks during sleep time between ticks
    std::vector<std::pair<packet::Type, std::string>> received_packets;

    /// @brief eid of the player that this session is associated with.
    EntityID eid;

    SocketError _classifySocketError(boost::system::error_code ec, const char* where);
};