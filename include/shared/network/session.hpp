#pragma once

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/hashed_index.hpp>
#include <boost/multi_index/member.hpp>

#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <array>
#include <queue>
#include <vector>
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
    SessionInfo(std::optional<std::string> client_name,
        std::optional<EntityID> client_eid, std::optional<bool> is_dungeon_master)
        : client_name(client_name), client_eid(client_eid), is_dungeon_master(is_dungeon_master) {}

    std::optional<std::string> client_name;
    std::optional<EntityID> client_eid;
    std::optional<bool> is_dungeon_master;
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
     * @returns true if there has been no fatal socket error, false otherwise
     */
    bool isOkay() const;

    /**
     * handles all of the incoming packets and puts the events in the event queue, which you
     * can get by calling getEvents
     * 
     * @returns list of all received events
     */
    std::vector<Event> handleAllReceivedPackets();

    /**
     * Connects to the ip
     * 
     * @returns true/false if the connection was successful
     */
    [[nodiscard("You should check if the connection was successful")]]
        bool connectTo(basic_resolver_results<class boost::asio::ip::tcp> endpoints);

    /**
     * Puts a packet in the queue of packets to send
     * 
     * @param packet The packet to send.
     */
    void sendPacket(std::shared_ptr<PackagedPacket> packet);

    /**
     * Sends an event on the socket after packaging it into
     * the packet format.
     * 
     * @param evt The event object to send
     */
    void sendEvent(Event evt);

    /**
     * Get the information associated with this session.
     */
    const SessionInfo& getInfo() const;

private:
    /// @brief true until there is a fatal error on the socket
    bool okay;

    std::optional<PacketHeader> prev_hdr;

    tcp::socket socket;

    std::vector<std::shared_ptr<PackagedPacket>> packets_to_send;

    std::array<char, NETWORK_BUFFER_SIZE> buffer;

    SessionInfo info;

    /**
     * Stores the received packet inside of the internal received_events
     * vector, so it can be retrieved later by the getEvents()
     * function.
     * 
     * @param type Type of the packet received
     * @param data Serialized format of the data received on the network.
     * @returns Event if the packet was an event, nullopt otherwise
     */
    std::optional<Event> _handleReceivedPacket(PacketType type, const std::string& data);

    /**
     * Classifies the error reported by boost::asio based on how we should respond to it.
     * 
     * @param ec Error code reported by boost::asio
     * @param where String representation of what was going on when the error occurred to
     * be displayed in an error message.
     */
    SocketError _classifySocketError(boost::system::error_code ec, const char* where);

    /**
     * @param bytes number of bytes to check for
     * @returns true if the socket has received enough bytes, false otherwise
     */
    bool socketHasEnoughBytes(std::size_t bytes);
};

/**
 * Used by server and client as the data stored in a boost::multi_index container
 */
struct SessionEntry {
    SessionEntry(EntityID id, 
        bool is_dungeon_master,
        boost::asio::ip::address ip, 
        std::shared_ptr<Session> session)
        : id(id), is_dungeon_master(is_dungeon_master), ip(ip), session(session) {}

    EntityID id;
    bool is_dungeon_master;
    boost::asio::ip::address ip;
    std::shared_ptr<Session> session;
};

/**
 * Hash function for a tcp ip address in boost, passed into the multi_index container
 * down below.
 * Borrowed from: https://stackoverflow.com/questions/22746359/unordered-map-with-ip-address-as-a-key
 */
struct ip_address_hash {
    size_t operator()(const boost::asio::ip::address& v) const { 
        if (v.is_v4())
            return v.to_v4().to_ulong();
        if (v.is_v6()) {
            auto const& range = v.to_v6().to_bytes();
            return boost::hash_range(range.begin(), range.end());
        }
        if (v.is_unspecified()) {
            // guaranteed to be random: chosen by fair dice roll
            return static_cast<size_t>(0x4751301174351161ul); 
        }
        return boost::hash_value(v.to_string());
    }
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
 */
using Sessions = boost::multi_index_container<
    SessionEntry,
    boost::multi_index::indexed_by<
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<IndexByID>,
            boost::multi_index::member<SessionEntry, EntityID, &SessionEntry::id>
        >,
        boost::multi_index::hashed_unique<
            boost::multi_index::tag<IndexByIP>,
            boost::multi_index::member<SessionEntry, boost::asio::ip::address, &SessionEntry::ip>,
            ip_address_hash
        >
    >
>;
