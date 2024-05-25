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
    this->okay = true;
}

Session::~Session() {
}

bool Session::isOkay() const {
    return this->okay;
}

const SessionInfo& Session::getInfo() const {
    return this->info;
}

std::vector<Event> Session::handleAllReceivedPackets() {
    if (!this->isOkay()) {
        return {};
    }

    std::vector<Event> events;

    while (true) {
        if (!prev_hdr.has_value()) {
            // we haven't already pulled in a header, see if there is a header waiting for us
            if (!socketHasEnoughBytes(sizeof(PacketHeader))) {
                return events;
            }

            boost::system::error_code ec;
            boost::asio::read(this->socket, boost::asio::buffer(this->buffer), 
                boost::asio::transfer_exactly(sizeof(PacketHeader)), ec);
            switch (_classifySocketError(ec, "receiving header")) {
                case SocketError::NONE: break;
                case SocketError::FATAL: 
                    this->okay = false;
                    return events;
            }

            this->prev_hdr = PacketHeader(static_cast<void*>(&this->buffer[0]));
        }

        // if (!socketHasEnoughBytes(this->prev_hdr->size)) {
        //     std::cout << "dont have enough bytes for " << this->prev_hdr->size << "\n";
        //     return events;
        // }

        boost::system::error_code ec;
        boost::asio::read(this->socket, boost::asio::buffer(this->buffer),
            boost::asio::transfer_exactly(this->prev_hdr->size), ec);

        switch (_classifySocketError(ec, "receiving data")) {
            case SocketError::NONE: break;
            case SocketError::FATAL:
                this->okay = false;
                std::cout << "setting ok to bad" << std::endl;
                return events;
        }

        std::string data(this->buffer.begin(), this->buffer.begin() + this->prev_hdr->size);
        auto event = _handleReceivedPacket(this->prev_hdr->type, data);
        if (event.has_value()) {
            events.push_back(event.value());
        }

        this->prev_hdr = {};
    }

    // shouldnt get here but for safety
    return events;
}

bool Session::connectTo(basic_resolver_results<class boost::asio::ip::tcp> endpoints) {
    boost::system::error_code ec;
    tcp::endpoint endpt = boost::asio::connect(socket, endpoints, ec);
    if (ec) {
        std::cerr << "ERROR Connecting: " << ec.what() << std::endl;
        return false;
    }
    std::cout << "connected\n";
    return true;
}

std::optional<Event> Session::_handleReceivedPacket(PacketType type, const std::string& data) {
    // First figure out if packet is event or non-event
    if (type == PacketType::Event) {
        auto event = deserialize<EventPacket>(data).event;
        return event;
    } else if (type == PacketType::ServerAssignEID) {
        this->info.client_eid = deserialize<ServerAssignEIDPacket>(data).eid;
        this->info.is_dungeon_master = deserialize<ServerAssignEIDPacket>(data).is_dungeon_master;
        std::cout << "Handling ServerAssignEID of " << *this->info.client_eid << "...\n";
        std::cout << "Is Dungeon Master? " << (*this->info.is_dungeon_master ? "true" : "false") << "...\n";
    } else if (type == PacketType::ClientDeclareInfo) {
        this->info.client_name = deserialize<ClientDeclareInfoPacket>(data).player_name;
        std::cout << "Handling ClientDeclareInfo from " << *this->info.client_name << "...\n";
    } else {
        std::cerr << "Unknown packet type received in Session::_addReceivedPacket " << (int) type << std::endl;
    }
    return {};
}

void Session::sendPacket(std::shared_ptr<PackagedPacket> packet) {
    if (!this->isOkay()) {
        return;
    }

    boost::system::error_code ec;
    boost::asio::write(this->socket, packet->toBuffer(), ec);

    switch (_classifySocketError(ec, "sending packet")) {
        case SocketError::NONE: break;
        // not currently handling retry
        case SocketError::FATAL: 
            this->okay = false;
            return;
    }
}

void Session::sendEvent(Event event) {
    this->sendPacket(PackagedPacket::make_shared(PacketType::Event, EventPacket(event)));
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

bool Session::socketHasEnoughBytes(std::size_t bytes) {
    boost::asio::socket_base::bytes_readable command(true);
    socket.io_control(command);
    std::size_t bytes_readable = command.get();

    return bytes_readable >= bytes;
}