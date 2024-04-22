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
#include "shared/utilities/serialize.hpp"


// Might want to move this later on to somewhere in the game code

/**
 * Enumeration for all of the different packet types that can be sent.
 * 
 * Server or Client prefix specifies which side sends that type of packet
 * 
 * Note: you need to update the below function validateType whenever you add a new
 * packet type!
 */
enum class PacketType: uint16_t {
    // Setup
    ServerLobbyBroadcast = 0, ///< Sent by the server via UDP broadcast saying it has a server open
    ClientDeclareInfo,   ///< Sent by the client after TCP handshake 
    ServerAssignEID,     ///< Sent by the server after TCP handshake, giving client its EID

    // Gameplay
    Event = 2000, ///< Client requesting server to perform specific input
};

/**
 * Header for any arbitrary packet on the network sent by our game.
 * Headers are sent as POD (plain-old-data), so the recipient can always
 * read exactly sizeof(Header), get the relevant information of the packet,
 * and then use boost::serialization to deserialize the more complicated
 * Packet types below.
 */
struct PacketHeader {
    /**
     * Constructor which makes a Header normally
     * 
     * @param size Size in bytes of the data portion of the packet (not inc header)
     * @param type Type of the packet, according to the PacketType enum
     */
    PacketHeader(uint16_t size, PacketType type): size{size}, type{type} {}

    /**
     * Converts this packet to network byte order. This should be called before sending
     * over the network.
     */
    void to_network() {
        this->size = htons(this->size);
        this->type = static_cast<PacketType>(htons(static_cast<uint16_t>(this->type))); 
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
    PacketHeader(void* buffer) {
        PacketHeader* buf_hdr = static_cast<PacketHeader*>(buffer);
        this->size = ntohs(buf_hdr->size);
        this->type = static_cast<PacketType>(ntohs(static_cast<uint16_t>(buf_hdr->type)));
    }

    /// @brief Size (in bytes) of the packet data (not including the header)
    uint16_t size;
    /// @brief What kind of packet this is, according to the Type enum class
    PacketType type;
};

/**
 * Packet sent by the server via UDP broadcast, announcing that there is a lobby available for
 * players to join.
 */
struct ServerLobbyBroadcastPacket {
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
struct ClientDeclareInfoPacket {
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
struct ServerAssignEIDPacket {
    /// @brief ID that the server is assigning to the client.
    EntityID eid;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & eid;
    }
};

struct EventPacket {
    Event event;
    
    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & event;
    }
};

/**
 * A class which wraps around a packet that has yet to be sent across the network.
 * Note: this class can only be instantiated as a shared_ptr using the provided friend
 * helper function
 */
class PackagedPacket { 
public:
    /**
     * We explicitly make the destructor availabale.
     */
    ~PackagedPacket() = default;

    /**
     * Factory function that creates a smart pointer for the packaged packet.
     * 
     * @param type of packet to make
     * @param packet actual packet data
     */
    template <class Packet>
    static std::shared_ptr<PackagedPacket> make_shared(PacketType type, Packet packet) {
        std::string data = serialize<Packet>(packet);

        PacketHeader hdr(data.size(), type);

        return std::shared_ptr<PackagedPacket>(new PackagedPacket(hdr, data));
    }

    /**
     * Converts the PackagedPacket into asio::buffer format. 
     * Note: it is important when doing an async write THAT THE PackagedPacket DOES NOT
     * GET DESTROYED BEFORE THE ASYNC WRITE ACTUALLY OCCURS. If the PackagedPacket is
     * destroyed before the buffer is read from, the underlying data will be deleted
     * and garbage will be written to the network socket.
     * 
     * Since we only expose a way to make this class as a shared_ptr, you should pass
     * this shared_ptr into any callback that needs to read from the packet
     * 
     * @return The packet in buffer format, which can easily be passed into boost::asio::write
     * or similar function.
     */
    std::array<boost::asio::const_buffer, 2> toBuffer() {
        return {
            boost::asio::buffer(&this->hdr, sizeof(PacketHeader)),
            boost::asio::buffer(this->data)
        };
    }

private:
    /**
     * Constructs a PackagedPacket for sending across the network. Converts the header
     * into network byte order, and sets the header size to be equal to the data, if
     * not already.
     * 
     * This constructor is PRIVATE because when making a packaged packet, you only ever want it
     * to be inside of a shared_ptr, so that you can pass the shared ptr into the async callback
     * function ensuring that the PackagedPacket will live as long as the async call needs it.
     * Use the provided make_shared function
     * 
     * @param hdr Header of the packet. This should not already be in network byte order, and 
     * the hdr.size does not need to be set.
     * @param data The string representation of the packet data. This will probably be the
     * return value of the serialize helper function.
     */
    PackagedPacket(PacketHeader hdr, std::string data)
        :hdr(hdr), data(data)
    {
        this->hdr.size = data.size();
        this->hdr.to_network();
    }

    /**
     * We delete the copy constructor and assignment operator so that this can only live inside of 
     * a shared ptr.
     */
    PackagedPacket(const PackagedPacket& a) = delete;
    PackagedPacket& operator=(const PackagedPacket& a) = delete;

    /// @brief Header of the packet to send
    PacketHeader hdr;
    /// @brief Data of the packet to send, in boost::serialize format
    std::string data;
};
