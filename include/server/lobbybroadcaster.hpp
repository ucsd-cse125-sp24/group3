#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

#include <thread>
#include <atomic>

#include "shared/network/packet.hpp"

using namespace boost::asio::ip;

/**
 * Class which abstracts away the logic of sending lobby broadcasts
 * announcing that there is a server available on this LAN
 */
class LobbyBroadcaster {
public:
    /**
     * Instantiates the Lobby Finder and spawns up a background thread which starts looking
     * for lobbies that are being broadcast over the LAN.
     */
    LobbyBroadcaster(boost::asio::io_context& io_context,
        packet::ServerLobbyBroadcast bcast_info);

    /**
     * Calls LobbyFinder::stopBroadcasting(), if it hasn't already been called.
     */
    ~LobbyBroadcaster();

    /**
     * Tell the broadcaster the current state of the lobby so it can broadcast
     * updated messages.
     */
    void setLobbyInfo(packet::ServerLobbyBroadcast bcast_info);

    /**
     * Tells the LobbyBroadcaster to stop advertising that there is a lobby open.
     * Called automatically in the destructor if not called explicitly beforehand.
     */
    void stopBroadcasting();

private:
    /**
     * Function that gets run in the background worker thread. This is what actually does
     * the socket operations and listens for the messages.
     */
    void _lobbyBroadcastWorker();

    /// @brief background thread object
    std::thread worker_thread;
    /// @brief boolean flag to keep searching. Set to false by stopBroadcasting().
    std::atomic_bool keep_broadcasting;
    /// @brief UDP socket that sends the broadcast packets
    udp::socket socket;

    /// @brief Lock to protect concurrent access to the broadcast info
    std::mutex mut;
    /// @brief info of the lobby to announce
    packet::ServerLobbyBroadcast bcast_info;
};
