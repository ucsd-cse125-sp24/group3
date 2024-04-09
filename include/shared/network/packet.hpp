#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <utility>

#include <boost/serialization/access.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/serialization/utility.hpp>

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
    ServerLobbyOpen = 0, ///< Sent by the server via UDP broadcast saying it has a server open
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
 */
struct Header {
    /// @brief Size (in bytes) of the packet (including the header)
    uint16_t size;
    /// @brief What kind of packet this is, according to the Type enum class
    Type type;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & this->size;
        ar & this->type;
    }
};

/**
 * Packet sent by the server via UDP broadcast, announcing that there is a lobby available for
 * players to join.
 */
struct ServerLobbyOpen {
    /// @brief Name of the server lobby
    std::string lobby_name;
    /// @brief How many clients are already in this lobby
    uint8_t slots_taken;
    /// @brief How many more clients can join this lobby
    uint8_t slots_avail;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        for (int i = 0; i < MAX_NAME_LEN; i++) {
            ar & this->lobby_name[i];
        }
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
    uint8_t slots_avail;
    
    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & players;
        ar & slots_avail;
    }
};

/**
 * Different actions that the clients can do while inside of a lobby
 */
enum class LobbyActionType : uint8_t {
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