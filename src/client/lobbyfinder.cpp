#include "client/lobbyfinder.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>

#include <iostream>
#include <mutex>

#include "shared/network/packet.hpp"

LobbyFinder::LobbyFinder(boost::asio::io_context& io_context)
    :lobby_discovery_socket(io_context, udp::endpoint(address_v4::any(), PORT)),
     worker_thread(&LobbyFinder::_searchForLobbyWorker, this),
     keep_searching(true)
{

}

LobbyFinder::~LobbyFinder() {
    if (this->keep_searching) {
        this->stopSearching();
    }
}

void LobbyFinder::stopSearching() {
    this->keep_searching = false;
    try {
        this->lobby_discovery_socket.close(); // this might throw an exception
        // Only try and join if the close call is successful.
        // It is preferable to potentially have a dangling thread in the background
        // than to completely block the application on this line, if closing the
        // socket messed up somehow.
        this->worker_thread.join();
    } catch (boost::system::system_error e) {
        std::cerr << "Error closing LobbyFinder socket: " << e.what() << std::endl;
    }
}

void LobbyFinder::_searchForLobbyWorker() {
    while (this->keep_searching) {
        try {
            std::size_t bytes_read = this->lobby_discovery_socket.receive_from(
                boost::asio::buffer(this->lobby_info_buf), this->endpoint_buf);

            if (!this->keep_searching) {
                return;
            }

            std::string packet(this->lobby_info_buf.begin(), this->lobby_info_buf.end());
            packet.resize(bytes_read);
            auto parsed_info = deserialize<packet::ServerLobbyBroadcast>(packet);

            std::unique_lock<std::mutex> lock(this->mut);
            this->lobbies_avail[this->endpoint_buf] = parsed_info;
        } catch (boost::system::system_error e) {
            std::cerr << "Error while searching for lobby: " << e.what() << std::endl;
        }
    }
}

Lobbies LobbyFinder::getFoundLobbies() {
    std::unique_lock<std::mutex> lock(this->mut);
    return this->lobbies_avail;
}