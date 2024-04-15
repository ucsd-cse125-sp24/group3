#pragma once

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

#include <array>
#include <unordered_map>
#include <thread>
#include <atomic>
#include <mutex>

#include "shared/network/packet.hpp"
#include "shared/utilities/config.hpp"

using namespace boost::asio::ip;

using Lobbies = std::unordered_map<udp::endpoint, packet::ServerLobbyBroadcast>;

/**
 * Class which abstracts away the logic of listening for lobby broadcast UDP packets
 * sent by the server when broadcasting that there is an available lobby on the LAN.
 */
class LobbyFinder {
public:
    /**
     * Instantiates the Lobby Finder but doesn't start searching.
     */
    LobbyFinder(boost::asio::io_context& io_context, GameConfig config);

    /**
     * Calls LobbyFinder::stopSearching(), if it hasn't already been called.
     */
    ~LobbyFinder();

    /**
     * Tells the lobby finder to start searching for lobbies. 
     */
    void startSearching();

    /**
     * Tells the LobbyFinder to stop trying to look for a lobby. Closes the socket, and exits 
     * the background thread. Called automatically in the destructor if not called explicitly
     * beforehand.
     */
    void stopSearching();

    /**
     * Gets all of the lobbies that the LobbyFinder has found. Explicitly returns a copy because
     * we dont want to pass around a reference to the underlying data member because the worker
     * thread could modify it later.
     */
    Lobbies getFoundLobbies();

private:
    /**
     * Function that gets run in the background worker thread. This is what actually does
     * the socket operations and listens for the messages.
     */
    void _searchForLobbyWorker();

    /// @brief background thread object
    std::thread worker_thread;
    /// @brief boolean flag to keep searching. Set to false by stopSearching().
    std::atomic_bool keep_searching;

    /// @brief UDP socket that listens for the broadcast packets
    udp::socket lobby_discovery_socket;
    /// @brief buffer in which the packet info is placed
    std::array<char, 512> lobby_info_buf;
    /// @brief buffer in which the server ip:port info is saved
    udp::endpoint endpoint_buf;

    /// @brief mutex to guard concurrent access to lobbies_avail vector
    std::mutex mut;
    /// @brief contains a record of all of the lobbies this obj has found
    Lobbies lobbies_avail;
};
