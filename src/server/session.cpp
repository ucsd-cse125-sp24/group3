#include "server/session.hpp"

#include <boost/asio.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <mutex>

#include "shared/network/packet.hpp"

using boost::asio::ip::tcp;

Session::Session(tcp::socket socket, EntityID eid)
    :socket(std::move(socket)),
     eid(eid)
{
    this->_receiveHdr();

    this->send(packagePacket(packet::Type::ServerAssignEID, packet::ServerAssignEID { .eid=eid }));
}

Session::~Session() {
}

void Session::send(PacketBuffer buf) {
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

void Session::_receiveHdr() {
    auto self(shared_from_this());
    boost::asio::async_read(this->socket, boost::asio::buffer(this->data),
        boost::asio::transfer_exactly(sizeof(packet::Header)),
        [this, self](boost::system::error_code ec, std::size_t length) {
            if (ec) {
                std::cerr << "Error reading header from client " << this->eid
                    << ": " << ec << "\n";
                this->_receiveHdr();
            } else {
                packet::Header hdr(static_cast<void*>(&this->data[0]));
                std::cout << "Session " << this->eid << " received packet hdr: {"
                    << hdr.size << "," << static_cast<int>(hdr.type) << "}" << std::endl;
                this->_receiveData(hdr);
            }
        });
}

void Session::_receiveData(packet::Header hdr) {
    auto self(shared_from_this());
    boost::asio::async_read(this->socket, boost::asio::buffer(this->data),
        boost::asio::transfer_exactly(hdr.size),
        [this, self, hdr](boost::system::error_code ec, std::size_t length) {
            if (ec) {
                std::cerr << "Error reading data from client " << this->eid
                    << ": " << ec << "\n";
                this->_receiveData(hdr);
            } else {
                std::string data(this->data, hdr.size);
                std::unique_lock<std::mutex> lock(this->mut);
                this->incoming_packets.push({hdr.type, data});
                this->_receiveHdr();
            }
        });
}
