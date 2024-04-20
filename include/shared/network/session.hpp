#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>
#include <string>
#include <optional>

#include "shared/network/packet.hpp"
#include "shared/utilities/typedefs.hpp"

using namespace boost::asio::ip;

/**
 * Enumeration to classify different kinds of networking errors depending on the kind
 * of action we should take. Note, we aren't currently classifying anything as RETRY.
 * IF we find an error that we are treating as FATAL but should be RETRY, then we can
 * use it.
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
 * Info about the client and server associated with this session.
 * 
 * The fields are optional because when the session is initially created,
 * some of the information may not have been sent yet, so we initialize
 * them to the "None" value.
 */
struct SessionInfo {
    std::optional<std::string> client_name;
    std::optional<EntityID> client_eid;
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
     * Constructs a new session, with the specified info 
     * 
     * @param socket The boost asio socket which is already open and contains
     * the connection.
     * @param info Session Information
     */
    Session(tcp::socket socket, SessionInfo info);
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
    std::vector<Event> getEvents();

    /**
     * Sends a packet on the socket
     * 
     * @param packet The packet to send.
     */
    void sendPacketAsync(std::shared_ptr<PackagedPacket> packet);

    /**
     * Sends an event on the socket after packaging it into
     * the packet format.
     * 
     * @param type Type of the event, should either be ServerDoEvent or
     * ClientRequestEvent
     * @param evt The event object to send
     */
    void sendEventAsync(PacketType type, Event evt);

    /**
     * Get the information associated with this session.
     */
    const SessionInfo& getInfo() const;

private:
    tcp::socket socket;

    std::vector<Event> received_events;

    SessionInfo info;

    /**
     * Stores the received packet inside of the internal received_events
     * vector, so it can be retrieved later by the getEvents()
     * function.
     * 
     * @param type Type of the packet received
     * @param data Serialized format of the data received on the network.
     */
    void _handleReceivedPacket(PacketType type, std::string data);

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

/**
 * Used by server and client as the data stored in a boost::multi_index container
 */
struct SessionWrapper {
    EntityID id;
    boost::asio::ip::address ip;
    std::weak_ptr<Session> session;
};

// Tag structs to let you index into a Sessions Multi_index map by a name
struct IndexByID {};
// Tag structs to let you index into a Sessions Multi_index map by a name
struct IndexByIP {};

/**
 * This creates a data structure which allows us to query for a Session by either
 * the id of the player associated with it, or by that player's tcp::endpoint information.
 * 
 * This stackoverflow post was helpful in understanding how to use boost::multi_index
 * https://stackoverflow.com/questions/39510143/how-to-use-create-boostmulti-index
 * 
 * HOW TO USE:
 * 
 * TODO: write tutorial
 */
using Sessions = boost::multi_index_container<
    SessionWrapper,
    boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<IndexByID>,
            boost::multi_index::member<SessionWrapper, EntityID, &SessionWrapper::id>
        >,
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<IndexByIP>,
            boost::multi_index::member<SessionWrapper, boost::asio::ip::address, &SessionWrapper::ip>
        >
    >
>;
