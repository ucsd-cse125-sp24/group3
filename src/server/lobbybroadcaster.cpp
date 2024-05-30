#include "server/lobbybroadcaster.hpp"

#include <boost/asio/io_context.hpp>

#include <iostream>
#include <thread>

#include "shared/game/sharedgamestate.hpp"
#include "shared/network/constants.hpp"
#include "shared/network/packet.hpp"
#include "shared/utilities/config.hpp"
#include "shared/utilities/serialize.hpp"

using namespace std::chrono_literals;
using namespace boost::asio::ip;

LobbyBroadcaster::LobbyBroadcaster(boost::asio::io_context& io_context, const GameConfig& config): // cppcheck-suppress uninitMemberVar
    socket(io_context),
    keep_broadcasting(false),
    config(config)
{

}

LobbyBroadcaster::~LobbyBroadcaster() {
    this->stopBroadcasting();
}

void LobbyBroadcaster::startBroadcasting(const ServerLobbyBroadcastPacket& bcast_info) {
    if (!this->keep_broadcasting) {
        this->bcast_info = bcast_info;
        this->keep_broadcasting = true;
        this->worker_thread = std::thread(&LobbyBroadcaster::_lobbyBroadcastWorker, this);
    }
}

void LobbyBroadcaster::setLobbyInfo(const Lobby& lobby_info) {
    std::unique_lock<std::mutex> lock(this->mut);
    this->bcast_info = ServerLobbyBroadcastPacket {
        .lobby_name = lobby_info.name,
        .slots_taken = static_cast<int>(lobby_info.numPlayersInLobby()),
        .slots_avail = static_cast<int>(lobby_info.max_players - lobby_info.numPlayersInLobby())
    };
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

    udp::endpoint endpt(address_v4::broadcast(), this->config.network.server_port);

    // Don't bother with packet headers here, because there is only one packet being sent over UDP
    // so we don't need to distinguish them.

    while (this->keep_broadcasting) {
        std::this_thread::sleep_for(1s);

        std::unique_lock<std::mutex> lock(this->mut);
        std::string packet_data = serialize<ServerLobbyBroadcastPacket>(this->bcast_info);
        this->socket.send_to(boost::asio::buffer(packet_data), endpt);
    }

    this->socket.close();
    std::cout << "No longer broadcasting lobby info." << std::endl;
}