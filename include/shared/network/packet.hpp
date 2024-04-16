#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <utility>
#include <ostream>
#include <sstream>
#include <iostream>

#include <boost/asio.hpp>

#include "shared/game/event.hpp"
#include "shared/network/constants.hpp"
#include "shared/utilities/typedefs.hpp"
#include "shared/utilities/serialize_macro.hpp"


// Might want to move this later on to somewhere in the game code

namespace packet {

/**
 * Enumeration for all of the different packet types that can be sent.
 * 
 * Server or Client prefix specifies which side sends that type of packet
 * 
 * Note: you need to update the below function validateType whenever you add a new
 * packet type!
 */
enum class Type: uint16_t {
    // Lobby Setup
    ServerLobbyBroadcast = 0, ///< Sent by the server via UDP broadcast saying it has a server open
    ClientDeclareInfo,   ///< Sent by the client after TCP handshake 
    ServerAssignEID,     ///< Sent by the server after TCP handshake, giving client its EID

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
     * Constructor which makes a Header normally
     * 
     * @param size Size in bytes of the data portion of the packet (not inc header)
     * @param type Type of the packet, according to the packet::Type enum
     */
    Header(uint16_t size, Type type): size{size}, type{type} {}

    /**
     * Converts this packet to network byte order. This should be called before sending
     * over the network.
     */
    void to_network() {
        this->size = htons(this->size);
        this->type = static_cast<Type>(htons(static_cast<uint16_t>(this->type))); 
    }

    /**
     * Constructor which takes a buffer received over the network and constructs
     * a Header object from it. It assumes that the packet is in network byte order
     * so this should not be used if the buffer contains a packet header not in network
     * byte order.
     * 
     * @param buffer The buffer received over the network containing the 4 bytes
     * for the header.
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

struct ClientRequestEvent {
    Event event;
    
    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & event;
    }
};

struct ServerDoEvent {
    Event event;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & event;
    }
};


}

/**
 * Helper function to easily serialize a packet's data into a string
 * to send over the network.
 * 
 * @param packet Packet object that you want to serialize to send across the network.
 * This should not include any header information, it should strictly be a struct
 * for packet data.
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
 * 
 * @param data String representation of a serialized packet recieved across the network.
 */
template <class Packet>
Packet deserialize(std::string data) {
    Packet parsed_info;
    std::istringstream stream(data);
    boost::archive::text_iarchive archive(stream);
    archive >> parsed_info;
    return parsed_info;
}

/**
 * A class which wraps around a packet that has yet to be sent across the network.
 */
class PackagedPacket { 
public:
    /**
     * Constructs a PackagedPacket for sending across the network. Converts the header
     * into network byte order, and sets the header size to be equal to the data, if
     * not already.
     * 
     * @param hdr Header of the packet. This should not already be in network byte order, and 
     * the hdr.size does not need to be set.
     * @param data The string representation of the packet data. This will probably be the
     * return value of the serialize helper function.
     */
    PackagedPacket(packet::Header hdr, std::string data)
        :hdr(hdr), data(data)
    {
        this->hdr.size = data.size();
        this->hdr.to_network();
    }

    /**
     * Converts the PackagedPacket into asio::buffer format. 
     * Note: it is important when doing an async write THAT THE PackagedPacket DOES NOT
     * GET DESTROYED BEFORE THE ASYNC WRITE ACTUALLY OCCURS. If the PackagedPacket is
     * destroyed before the buffer is read from, the underlying data will be deleted
     * and garbage will be written to the network socket.
     * 
     * @return The packet in buffer format, which can easily be passed into boost::asio::write
     * or similar function.
     */
    std::array<boost::asio::const_buffer, 2> toBuffer() {
        return {
            boost::asio::buffer(&this->hdr, sizeof(packet::Header)),
            boost::asio::buffer(this->data)
        };
    }
private:
    /// @brief Header of the packet to send
    packet::Header hdr;
    /// @brief Data of the packet to send, in boost::serialize format
    std::string data;
};

/**
 * Helper function that packages a packet as a collection of boost buffers, which can
 * then be sent into a write socket call. This conveniently puts it inside of a shared
 * ptr which should be passed into the boost async callback 
 * 
 * @param type Type of the packet
 * @param packet Packet data to send
 */
template <class Packet>
std::shared_ptr<PackagedPacket> packagePacket(packet::Type type, Packet packet) {
    std::string data = serialize<Packet>(packet);

    packet::Header hdr(data.size(), type);

    auto pkt = std::make_shared<PackagedPacket>(hdr, data);

    return pkt;
}