#include "server/server.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <iostream>
#include <fstream>
#include <ostream>
#include <thread>

#include "shared/network/packet.hpp"
#include "shared/network/constants.hpp"

using namespace std::chrono_literals;
using namespace boost::asio::ip;

Server::Server(boost::asio::io_context& io_context)
	:lobby_broadcast_socket(io_context),
	 keep_broadcasting_lobby(true),
	 lobby_broadcast_thread(&Server::_broadcastLobbyWorker, this)
{
	std::this_thread::sleep_for(100s);
	this->keep_broadcasting_lobby = false;
	this->lobby_broadcast_thread.join();
}

void Server::_broadcastLobbyWorker() {
	// Start broadcasting that there is a lobby available
	boost::system::error_code err;
	this->lobby_broadcast_socket.open(udp::v4(), err);

	if (err) {
		// TODO: add config file that lets you disable this
		std::cerr << "Error opening lobby broadcast socket: " << err.what() << "\n";
		std::cerr << "Clients will not be able to automatically discover this server.\n";
		std::terminate();
	}

	this->lobby_broadcast_socket.set_option(udp::socket::reuse_address(true));
	this->lobby_broadcast_socket.set_option(boost::asio::socket_base::broadcast(true));

	udp::endpoint endpt(address_v4::broadcast(), PORT);

	std::string lobby_name = "Test Lobby";

	// Don't bother with headers here, because there is only one packet being sent over UDP
	// so we don't need to distinguish them.
	packet::ServerLobbyBroadcast data { .lobby_name=lobby_name, .slots_taken=0, .slots_avail=4 };
	std::string packet_data = serialize<packet::ServerLobbyBroadcast>(data);

	while (this->keep_broadcasting_lobby) {
		std::cout << "Broadcasting lobby info for " << data.lobby_name
			<< " (" << static_cast<int>(data.slots_taken) << "/" 
			<< static_cast<int>(data.slots_avail) + static_cast<int>(data.slots_taken) << ")\n";
		lobby_broadcast_socket.send_to(boost::asio::buffer(packet_data), endpt);
		std::this_thread::sleep_for(1s);
	}

	this->lobby_broadcast_socket.close();
	std::cout << "No longer broadcasting lobby info.\n";
}