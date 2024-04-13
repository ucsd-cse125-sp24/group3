#include "shared/network/session.hpp"

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
    std::cout << "New Client Session Established (eid=" << eid << ")" << std::endl;
}

Session::~Session() {
}

void Session::startListen() {
    // This starts a chain that will continue on and on
    receivePacketAsync();
}

void Session::addReceivedPacket(packet::Type type, std::string data) {
    this->received_packets.push_back({type, data});
}

std::vector<std::pair<packet::Type, std::string>> Session::getAllReceivedPackets() {
    std::vector<std::pair<packet::Type, std::string>> vec;

    std::swap(vec, this->received_packets);

    return vec;
}

void Session::sendPacketAsync(std::shared_ptr<PackagedPacket> packet) {
    // pass packet shared ptr into closure capture list to keep it alive until written to buffer
    boost::asio::async_write(socket, packet->toBuffer(),
        [this, packet](boost::system::error_code ec, std::size_t /*length*/) {
            switch (_classifySocketError(ec, "sending packet")) {
            case SocketError::NONE:
                std::cout << "Sent packet successfully." << std::endl;
                break;
            case SocketError::FATAL:
                return;
            case SocketError::RETRY:
                sendPacketAsync(packet);
                return;
            }
        });
}

void Session::receivePacketAsync() {
    auto self(shared_from_this());
    const std::size_t BUF_SIZE = 10000;
    auto buf = std::make_shared<std::array<char, BUF_SIZE>>();

    boost::asio::async_read(socket, boost::asio::buffer(&buf.get()[0], BUF_SIZE),
        boost::asio::transfer_exactly(sizeof(packet::Header)),
        [this, buf, self](boost::system::error_code ec, std::size_t length) {

            switch (_classifySocketError(ec, "receiving header")) {
            case SocketError::NONE:
                break;
            case SocketError::FATAL:
                return;
            }

            packet::Header hdr(static_cast<void*>(&buf.get()[0]));
            std::cout << "Received packet hdr: {"
                << hdr.size << "," << static_cast<int>(hdr.type) << "}" << std::endl;

            boost::asio::async_read(socket, boost::asio::buffer(buf.get(), BUF_SIZE),
                boost::asio::transfer_exactly(hdr.size),
                [this, hdr, buf, self](boost::system::error_code ec,
                    std::size_t length)
                {
                    switch (_classifySocketError(ec, "receiving data")) {
                    case SocketError::NONE:
                        break;
                    case SocketError::FATAL:
                        return;
                    }
                    std::cout << "bytes read: " << length << std::endl;

                    std::string data(buf->begin(), buf->begin() + hdr.size);
                    self->addReceivedPacket(hdr.type, data);
                    std::cout << "received full packet data" << std::endl;
                    receivePacketAsync();
                });

        });
}

SocketError Session::_classifySocketError(boost::system::error_code ec, const char* where) {
    if (!ec) {
        return SocketError::NONE;
    }

    std::cerr << "Socket error " << where << ": " << ec.message() << std::endl;

    // Might need to use more fine-tuned error types later on, but just going to assume 
    // all fatal right now
    return SocketError::FATAL;


    // if (ec == boost::asio::error::connection_reset ||
    //     ec == boost::asio::error::eof ||
    //     ec == boost::asio::error::access_denied ||
    //     ec == boost::asio::error::bad_descriptor ||
    //     ec == boost::asio::error::broken_pipe) {
    //     return SocketError::FATAL;
    // } else {
    //     return SocketError::RETRY;
    // }
}