#include "server/lobbybroadcaster.hpp"

#include <boost/asio/io_context.hpp>

#include <iostream>
#include <thread>

#include "shared/network/constants.hpp"
#include "shared/network/packet.hpp"

using namespace std::chrono_literals;
using namespace boost::asio::ip;

LobbyBroadcaster::LobbyBroadcaster(boost::asio::io_context& io_context, packet::ServerLobbyBroadcast bcast_info)
    :socket(io_context),
     keep_broadcasting(true),
     worker_thread(&LobbyBroadcaster::_lobbyBroadcastWorker, this),
     bcast_info{bcast_info}
{

}

LobbyBroadcaster::~LobbyBroadcaster() {
    this->stopBroadcasting();
}

void LobbyBroadcaster::setLobbyInfo(packet::ServerLobbyBroadcast bcast_info) {
    std::unique_lock<std::mutex> lock(this->mut);
    this->bcast_info = bcast_info;
}

void LobbyBroadcaster::stopBroadcasting() {
    if (this->keep_broadcasting) {
        this->keep_broadcasting = false;
    }
}

void LobbyBroadcaster::_lobbyBroadcastWorker() {
    // Start broadcasting that there is a lobby available
    boost::system::error_code err;
    this->socket.open(udp::v4(), err);

    if (err) {
        // TODO: add config file that lets you disable this
        std::cerr << "Error opening lobby broadcast socket: " << err.what() << "\n";
        std::cerr << "Clients will not be able to automatically discover this server." << std::endl;
        return; 
    }

    this->socket.set_option(udp::socket::reuse_address(true));
    this->socket.set_option(boost::asio::socket_base::broadcast(true));

    udp::endpoint endpt(address_v4::broadcast(), PORT);

    // Don't bother with packet headers here, because there is only one packet being sent over UDP
    // so we don't need to distinguish them.

    while (this->keep_broadcasting) {
        std::this_thread::sleep_for(1s);

        std::unique_lock<std::mutex> lock(this->mut);
        std::string packet_data = serialize<packet::ServerLobbyBroadcast>(this->bcast_info);
        std::cout << "Broadcasting lobby info for " << this->bcast_info.lobby_name
            << " (" << this->bcast_info.slots_taken << "/"
            << this->bcast_info.slots_taken + this->bcast_info.slots_avail
            << ")" << std::endl;
        this->socket.send_to(boost::asio::buffer(packet_data), endpt);
    }

    this->socket.close();
    std::cout << "No longer broadcasting lobby info." << std::endl;
}