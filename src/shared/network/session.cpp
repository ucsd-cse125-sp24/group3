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
