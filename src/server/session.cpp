#include "server/session.hpp"

#include <boost/asio.hpp>

#include <iostream>
#include <memory>
#include <string>

#include "shared/network/packet.hpp"

using boost::asio::ip::tcp;

Session::Session(tcp::socket socket, EntityID eid)
    :socket(std::move(socket)),
     eid(eid)
{
    this->do_read_background();

    this->do_write(packagePacket(packet::Type::ServerAssignEID, packet::ServerAssignEID { .eid=eid }));
}

Session::~Session() {

}

void Session::do_write(PacketBuffer buf) {
    // make sure current instance of Session remains alive if pending async ops
    auto self(shared_from_this());
    boost::asio::write(this->socket, buf,
        [this, self](boost::system::error_code ec, std::size_t /*length*/)
        {
            if (!ec)
            {
            }
        });
}

void Session::do_read_background() {
    // make sure current instance of Session remains alive if pending async ops
    auto self(shared_from_this());
    this->socket.async_read_some(boost::asio::buffer(this->data, max_length),
        [this, self](boost::system::error_code ec, std::size_t length)
        {
            if (ec) {
                std::cerr << "Error reading packet from client " << this->eid
                    << ": " << ec << std::endl;
            }

            this->do_read_background(); // read again
        });
}