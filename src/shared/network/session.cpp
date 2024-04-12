#include "shared/network/session.hpp"

#include <boost/asio.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <mutex>

#include "shared/network/packet.hpp"
#include "shared/network/socket.hpp"

using boost::asio::ip::tcp;

Session::Session(tcp::socket socket, EntityID eid)
    :socket(std::move(socket)),
     eid(eid)
{
    std::cout << "New Client Session Established (eid=" << eid << ")" << std::endl;
    sendPacketAsync(this->socket,
        packagePacket(packet::Type::ServerAssignEID, packet::ServerAssignEID { .eid=eid }));
}

Session::~Session() {
}

void Session::startListen() {
    // This starts a chain that will continue on and on
    receivePacketAsync(this->socket, shared_from_this());
}

void Session::addReceivedPacket(packet::Type type, std::string data) {
    this->received_packets.push_back({type, data});
}

std::vector<std::pair<packet::Type, std::string>> Session::getAllReceivedPackets() {
    std::vector<std::pair<packet::Type, std::string>> vec;

    std::swap(vec, this->received_packets);

    return vec;
}
