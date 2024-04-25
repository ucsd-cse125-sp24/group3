#pragma once

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_context.hpp>

#include <thread>
#include <atomic>
#include <unordered_map>
#include <memory>
#include <chrono>

#include "server/lobbybroadcaster.hpp"
#include "shared/network/session.hpp"
#include "shared/utilities/config.hpp"
#include "shared/utilities/typedefs.hpp"
#include "server/game/servergamestate.hpp"

using boost::asio::ip::tcp;

/// Represents a list of events from a certain client with a specified ID
using EventList = std::vector<std::pair<EntityID, Event>>;

class Server {
public:
    Server(boost::asio::io_context& io_context, GameConfig config);

    static EntityID genNewEID();

    /**
     * Do one game tick
     * 
     * @returns Time in MS to wait until starting next tick
     */
    std::chrono::milliseconds doTick();

    EventList getAllClientEvents();

    void updateGameState(const EventList& events);

    void sendUpdateToAllClients(Event event);

private:
    /// @brief EID that is reserved for the Server / World itself.
    EntityID world_eid;

    /// @brief Broadcaster which spawns up another thread advertising this lobby
    LobbyBroadcaster lobby_broadcaster;

    void _doAccept();

    tcp::acceptor acceptor;
    tcp::socket socket;

    /**
     * Takes the address of an incoming TCP Connection, and handles it by
     * putting it into the Sessions map, taking care if whether this is a reconnection
     * or initial connection.
     * 
     * Precondition: this->socket holds a socket for a new connection that was just
     * established by the tcp::acceptor.
     * 
     * Postcondition: this->socket is no longer a valid socket as it has been moved
     * into the Session.
     * 
     * @returns a shared_ptr to the Session that was created/reestablished
     */
    std::shared_ptr<Session> _handleNewSession(boost::asio::ip::address addr);

    /// @brief Mapping from either player id or ip to session
    Sessions sessions;

    /// @brief Master copy of the ServerGameState, living on the server
    ServerGameState state;
};
