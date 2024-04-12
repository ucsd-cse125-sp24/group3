#include "server/session.hpp"

#include <boost/asio.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <mutex>

#include "shared/network/packet.hpp"
#include "shared/network/gamesocket.hpp"

using boost::asio::ip::tcp;

Session::Session(GameSocket&& gsocket, EntityID eid)
    :gsocket(std::move(gsocket)),
     eid(eid)
{
    std::cout << "New Client Session Established (eid=" << eid << ")" << std::endl;
    this->gsocket.send(packagePacket(packet::Type::ServerAssignEID, packet::ServerAssignEID { .eid=eid }));
}

Session::~Session() {
}
