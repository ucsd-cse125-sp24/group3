#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include <string>

#include "shared/network/packet.hpp"

using namespace boost::asio::ip;

/**
 * Enumeration to classify different kinds of networking errors depending on the kind
 * of action we should take
 */
enum SocketError {
    /// @brief No error, everything is good
    NONE,
    /// @brief We cannot recover from this error, so the socket is probably dead
    FATAL,
    /// @brief There was an error, but it doesn't compromise the socket connection
    RETRY,
};

/**
 * A class which wraps around the concept of a Client <-> Server relationship. This
 * works from both the client perpsective and the Server perspective. It essentially
 * provides a wrapper around a tcp::socket and lets us easily send and receive
 * our packet types.
 */
class Session : public std::enable_shared_from_this<Session> {
public:
    /**
     * Constructs a new session, with the specified client eid
     * 
     * @param socket The boost asio socket which is already open and contains
     * the connection.
     */
    Session(tcp::socket socket);
    ~Session();

    /**
     * Starts listening for packets on the socket 
     */
    void startListen();

    void connectTo(basic_resolver_results<class boost::asio::ip::tcp> endpoints);

    /**
     * Gets all of the received packets since the last time this function was called.
     * 
     * @returns received packets on the socket
     */
    std::vector<std::pair<packet::Type, std::string>> getAllReceivedPackets();

    /**
     * Sends a packet on the socket
     * 
     * @param packet The packet to send.
     */
    void sendPacketAsync(std::shared_ptr<PackagedPacket> packet);

private:
    tcp::socket socket;

    // TODO: determine if we need a mutex to proect this or not,
    // might not because we will only be checking received packets
    // during the game tick, and will only be running background async 
    // tasks during sleep time between ticks
    std::vector<std::pair<packet::Type, std::string>> received_packets;

    /**
     * Stores the received packet inside of the internal received_packets
     * vector, so it can be retrieved later by the getAllReceivedPackets()
     * function.
     * 
     * @param type Type of the packet received
     * @param data Serialized format of the data received on the network.
     */
    void _addReceivedPacket(packet::Type type, std::string data);

    /**
     * Sets up one async callback to receive a packet. This callback ends up calling
     * receivePacketAsync again, if there was no FATAL error, meaning this only needs
     * be called once, and then another time if there was a FATAL error and we are
     * sure we fixed the error on the socket.
     */
    void _receivePacketAsync();

    /**
     * Classifies the error reported by boost::asio based on how we should respond to it.
     * 
     * @param ec Error code reported by boost::asio
     * @param where String representation of what was going on when the error occurred to
     * be displayed in an error message.
     */
    SocketError _classifySocketError(boost::system::error_code ec, const char* where);
};