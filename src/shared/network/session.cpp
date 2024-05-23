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

bool Session::connectTo(basic_resolver_results<class boost::asio::ip::tcp> endpoints) {
    boost::system::error_code ec;
    tcp::endpoint endpt = boost::asio::connect(socket, endpoints, ec);
    if (ec) {
        std::cerr << "ERROR Connecting: " << ec.what() << std::endl;
        return false;
    }
    return true;
}

void Session::_handleReceivedPacket(PacketType type, const std::string& data) {
    // First figure out if packet is event or non-event
    if (type == PacketType::Event) {
        auto event = deserialize<EventPacket>(data).event;
        this->received_events.push_back(event);
    } else if (type == PacketType::ServerAssignEID) {
        this->info.client_eid = deserialize<ServerAssignEIDPacket>(data).eid;
        std::cout << "Handling ServerAssignEID of " << *this->info.client_eid << "...\n";
    } else if (type == PacketType::ClientDeclareInfo) {
        this->info.client_name = deserialize<ClientDeclareInfoPacket>(data).player_name;
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
                break;
            case SocketError::FATAL:
                return;
            case SocketError::RETRY:
                sendPacketAsync(packet);
                return;
            }
        });
}

void Session::sendEventAsync(Event event) {
    this->sendPacketAsync(PackagedPacket::make_shared(PacketType::Event, EventPacket(event)));
}

void Session::_receivePacketAsync() {
    auto self(shared_from_this());
    const std::size_t BUF_SIZE = NETWORK_BUFFER_SIZE;
    auto buf = std::make_shared<std::array<char, BUF_SIZE>>();

    boost::asio::async_read(socket, boost::asio::buffer(&buf.get()[0], BUF_SIZE),
        boost::asio::transfer_exactly(sizeof(PacketHeader)),
        [this, buf, self](boost::system::error_code ec, std::size_t length) {
            switch (_classifySocketError(ec, "receiving header")) {
            case SocketError::NONE:
                break;
            case SocketError::FATAL:
                return;
            }

            PacketHeader hdr(static_cast<void*>(&buf.get()[0]));

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