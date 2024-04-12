#include "shared/network/gamesocket.hpp"

#include <boost/asio/ip/tcp.hpp>

#include <memory>
#include <iostream>

using namespace boost::asio::ip;

GameSocket::GameSocket(tcp::socket&& socket, int identifier)
    :socket(std::move(socket)),
     identifier(identifier),
     last_error(error_type::OKAY)
{
    // std::cout << "c1" << std::endl;
    this->_receiveHdr();
}

GameSocket::GameSocket(GameSocket&& other)
    :socket(std::move(other.socket)),
     incoming_packets(std::move(other.incoming_packets)),
     identifier(std::move(other.identifier)),
     last_error(std::move(other.last_error))
{
    // std::cout << "c2" << std::endl;
}

GameSocket::~GameSocket() {
    // std::cout << "d1" << std::endl;
}

Packets GameSocket::receive() {
    std::unique_lock<std::mutex> lock(this->mut);

    Packets result;
    // makes this->incoming_packets empty because we are swapping with an empty vector
    std::swap(result, this->incoming_packets);
    return result;
}

void GameSocket::send(PacketBuffer buf) {
    if (this->last_error == FATAL) {
        std::cerr << "GS" << this->identifier << " cannot send because of prev FATAL error" << std::endl;
        return;
    }

    boost::asio::async_write(this->socket, buf,
        [this](boost::system::error_code ec, std::size_t /*length*/) {
            switch (this->_classifyError(ec, "sending packet")) {
                case error_type::OKAY:
                    std::cout << "GS" << this->identifier << " sent packet." << std::endl;
                    break;
                case error_type::FATAL: 
                    return;
                case error_type::RETRY:
                    this->_receiveHdr();
                    return;
            }
        });
}

void GameSocket::_receiveHdr() {
    boost::asio::async_read(this->socket, boost::asio::buffer(this->data),
        boost::asio::transfer_exactly(sizeof(packet::Header)),
        [this](boost::system::error_code ec, std::size_t length) {
            switch (this->_classifyError(ec, "receiving header")) {
                case error_type::OKAY:
                    break;
                case error_type::FATAL: 
                    return;
                case error_type::RETRY:
                    this->_receiveHdr();
                    return;
            }

            packet::Header hdr(static_cast<void*>(&this->data[0]));
            std::cout << "GS" << this->identifier << " received packet hdr: {"
                << hdr.size << "," << static_cast<int>(hdr.type) << "}" << std::endl;

            this->_receiveData(hdr);
        });
}

void GameSocket::_receiveData(packet::Header hdr) {
    std::cout << "c1" << std::endl;
    // if (hdr.size > max_length) {
    //     std::cout << "ERROR: cannot read packet longer than " << max_length << std::endl;
    //     this->_receiveHdr();
    // }

    boost::asio::async_read(this->socket, boost::asio::buffer(this->data),
        boost::asio::transfer_exactly(hdr.size),
        [this, hdr](boost::system::error_code ec, std::size_t length) {
            switch (this->_classifyError(ec, "receiving data")) {
                case error_type::OKAY:
                    break;
                case error_type::FATAL: 
                    return;
                case error_type::RETRY:
                    this->_receiveData(hdr);
                    return;
            }

            std::string data(this->data, hdr.size);
            std::unique_lock<std::mutex> lock(this->mut);
            this->incoming_packets.push_back({hdr.type, data});
            lock.unlock();
            std::cout << "GS " << this->identifier << " received full packet data" << std::endl;
            this->_receiveHdr();
        });
}

GameSocket::error_type GameSocket::_classifyError(
    boost::system::error_code ec, const char* where)
{

    if (!ec) {
        this->last_error == error_type::OKAY;
        return this->last_error;
    }

    std::cerr << "GS" << this->identifier << " error " << where << ": " << ec.message() << std::endl;

    // Might need to use more fine-tuned error types later on, but these two seem an okay start
    // for now.
    if (ec == boost::asio::error::connection_reset ||
        ec == boost::asio::error::eof ||
        ec == boost::asio::error::access_denied ||
        ec == boost::asio::error::bad_descriptor ||
        ec == boost::asio::error::broken_pipe) {
        std::cout << "FATAL ERROR for GS" << this->identifier << std::endl;
        this->last_error == error_type::FATAL;
    } else {
        this->last_error == error_type::RETRY;
    }

    return this->last_error;
}