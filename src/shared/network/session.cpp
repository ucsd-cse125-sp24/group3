#include "shared/network/session.hpp"

#include <boost/asio.hpp>

#include <iostream>
#include <memory>
#include <string>
#include <mutex>

#include "shared/network/packet.hpp"

using namespace boost::asio::ip;

Session::Session(tcp::socket socket, SessionInfo info):
    socket(std::move(socket)),
    info(info)
{
}

Session::~Session() {
}

const SessionInfo& Session::getInfo() const {
    return this->info;
}

void Session::startListen() {
    // This starts a chain that will continue on and on
    _receivePacketAsync();
}

void Session::connectTo(basic_resolver_results<class boost::asio::ip::tcp> endpoints) {
    tcp::endpoint endpt = boost::asio::connect(socket, endpoints);
}

void Session::_handleReceivedPacket(packet::Type type, std::string data) {
    // First figure out if packet is event or non-event
    if (type == packet::Type::ClientRequestEvent || type == packet::Type::ServerDoEvent) {
        std::cout << "Handling event...\n";
        auto event = deserialize<Event>(data);
        std::cout << event.type << std::endl;
        this->received_events.push_back(deserialize<Event>(data));
    } else if (type == packet::Type::ServerAssignEID) {
        this->info.client_eid = deserialize<packet::ServerAssignEID>(data).eid;
        std::cout << "Handling ServerAssignEID of " << *this->info.client_eid << "...\n";
    } else if (type == packet::Type::ClientDeclareInfo) {
        this->info.client_name = deserialize<packet::ClientDeclareInfo>(data).player_name;
        std::cout << "Handling ClientDeclareInfo from " << *this->info.client_name << "...\n";
    } else {
        std::cerr << "Unknown packet type received in Session::_addReceivedPacket" << std::endl;
    }
}

std::vector<Event> Session::getEvents() {
    std::vector<Event> vec;

    std::swap(vec, this->received_events);

    return vec;
}

void Session::sendPacketAsync(std::shared_ptr<PackagedPacket> packet) {
    auto self = shared_from_this();

    // pass packet shared ptr into closure capture list to keep it alive until written to buffer
    boost::asio::async_write(socket, packet->toBuffer(),
        [this, packet, self](boost::system::error_code ec, std::size_t /*length*/) {
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

void Session::sendEventAsync(packet::Type type, Event evt) {
    std::shared_ptr<PackagedPacket> packet = nullptr;

    std::cout << "TYPE1:" << evt.type << std::endl;

    if (type == packet::Type::ServerDoEvent) {
        packet = PackagedPacket::make_shared(type, packet::ServerDoEvent {
            .event = evt
        });
    } else if (type == packet::Type::ClientRequestEvent) {
        packet = PackagedPacket::make_shared(type, packet::ClientRequestEvent {
            .event = evt
        });
    } else {
        std::cerr << "Error: non event packet passed into Session::sendEventAsync(). Ignoring"
            << std::endl;
        return;
    }

    this->sendPacketAsync(packet);
}

void Session::_receivePacketAsync() {
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

                    std::string data(buf->begin(), buf->begin() + hdr.size);
                    self->_handleReceivedPacket(hdr.type, data);
                    _receivePacketAsync();
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