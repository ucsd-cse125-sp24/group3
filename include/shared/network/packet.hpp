#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <utility>
#include <ostream>
#include <sstream>

#include <boost/asio.hpp>
#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include "shared/network/constants.hpp"

// Helper macro to reduce boilerplate in making boost::serialize-able structs
#define DEF_SERIALIZE \
    friend class boost::serialization::access; \
    template<class Archive> \
    void serialize

// Might want to move this later on to somewhere in the game code
using EntityID = uint32_t;

namespace packet {

/**
 * Enumeration for all of the different packet types that can be sent.
 * 
 * Server or Client prefix specifies which side sends that type of packet
 */
enum class Type: uint16_t {
    // Lobby Setup
    ServerLobbyBroadcast = 0, ///< Sent by the server via UDP broadcast saying it has a server open
    ClientDeclareInfo,   ///< Sent by the client after TCP handshake 
    ServerAssignEID,     ///< Sent by the server after TCP handshake, giving client its EID
    ServerLobbyInfo,     ///< Sent periodically to all clients in lobby telling lobby info

    // Loading
    ServerSendGameState = 1000, ///< Serialized repr of entire GameState, sent to clients

    // Gameplay
    ClientRequestEvent = 2000, ///< Client requesting server to perform specific input
    ServerDoEvent,             ///< Server telling clients what events have occurred.
};

/**
 * Header for any arbitrary packet on the network sent by our game.
 * Headers are sent as POD (plain-old-data), so the recipient can always
 * read exactly sizeof(Header), get the relevant information of the packet,
 * and then use boost::serialization to deserialize the more complicated
 * Packet types below.
 */
struct Header {
    /**
     * Constructor which makes a Header with values in network byte order
     */
    Header(uint16_t size, Type type) {
        this->size = htons(size);
        this->type = static_cast<Type>(htons(static_cast<uint16_t>(type))); 
    }

    /**
     * Constructor which takes a buffer received over the network and constructs
     * a Header object from it.
     */
    Header(void* buffer) {
        Header* buf_hdr = static_cast<Header*>(buffer);
        this->size = ntohs(buf_hdr->size);
        this->type = static_cast<Type>(ntohs(static_cast<uint16_t>(buf_hdr->type)));
    }

    /// @brief Size (in bytes) of the packet data (not including the header)
    uint16_t size;
    /// @brief What kind of packet this is, according to the Type enum class
    Type type;
};

/**
 * Packet sent by the server via UDP broadcast, announcing that there is a lobby available for
 * players to join.
 */
struct ServerLobbyBroadcast {
    /// @brief Name of the server lobby
    std::string lobby_name;
    /// @brief How many clients are already in this lobby
    int slots_taken;
    /// @brief How many more clients can join this lobby
    int slots_avail;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & this->lobby_name;
        ar & this->slots_taken;
        ar & this->slots_avail;
    }
};

/**
 * Packet sent by the client to the server after the TCP handshake, telling
 * the server what their information is. Currently this is just a name.
 */
struct ClientDeclareInfo {
    /// @brief Name that the client wishes to be called by.
    std::string player_name;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & this->player_name;
    }
};

/**
 * Packet sent by the server to a client after the TCP handshake, telling the
 * client what their player's entity ID is.
 */
struct ServerAssignEID {
    /// @brief ID that the server is assigning to the client.
    EntityID eid;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & eid;
    }
};

/**
 * Packet sent periodically by the server to clients in a lobby, informing them of
 * who else is in the lobby and everyone's ready statuses.
 */
struct ServerLobbyInfo {
    /// @brief Contains (Entity ID, name, ready_status) information for each player
    std::vector<std::tuple<EntityID, std::string, bool>> players;
    /// @brief How many more spots there are available in the lobby
    int slots_avail;
    
    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & players;
        ar & slots_avail;
    }
};

/**
 * Different actions that the clients can do while inside of a lobby
 */
enum class LobbyActionType {
    Leave = 0,
    SetReady,
    SetNotReady
};

/**
 * Packet sent by the client to the server whenever they make an action in a lobby.
 */
struct ClientLobbyAction {
    LobbyActionType action;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & action;
    }
};

// TODO: packets for the actual game itself
// https://docs.google.com/document/d/1gkvuVnrpik86YUdQWANHn61yHgOpOSGWT6aXjq7orYg/edit

}

/**
 * Helper function to easily serialize a packet's data into a string
 * to send over the network.
 */
template<class Packet>
std::string serialize(Packet packet) {
    std::ostringstream archive_stream;
    boost::archive::text_oarchive archive(archive_stream);
    archive << packet;
    return archive_stream.str();
}

/**
 * Helper function to easily deserialize a string received over the network into
 * a packet.
 */
template <class Packet>
Packet deserialize(std::string data) {
    Packet parsed_info;
    std::istringstream stream(data);
    boost::archive::text_iarchive archive(stream);
    archive >> parsed_info;
    return parsed_info;
}

using PacketBuffer = std::array<boost::asio::const_buffer, 2>;

/**
 * Helper function that packages a packet as a collection of boost buffers, which can
 * then be sent into a write socket call.
 */
template <class Packet>
PacketBuffer packagePacket(packet::Type type, Packet packet) {
    std::string data = serialize<Packet>(packet);
    packet::Header hdr(data.size(), type);

    PacketBuffer bufs = {
        boost::asio::buffer(&hdr, sizeof(packet::Header)),
        boost::asio::buffer(data)
    };

    return bufs;
}