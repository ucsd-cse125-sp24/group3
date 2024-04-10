#include "client/client.hpp"

#include <boost/asio/io_context.hpp>
#include <iostream>
#include <thread>

#include "shared/network/constants.hpp"

using namespace boost::asio::ip;
using namespace std::chrono_literals;

Client::Client(boost::asio::io_context& io_context)
	:lobby_discovery_socket(io_context, udp::endpoint(udp::v4(), PORT))
{
	std::string in_data;
	udp::endpoint server_endpoint;

	this->lobby_discovery_socket.async_receive_from(
		boost::asio::buffer(in_data), server_endpoint,
		[&in_data, &server_endpoint](const boost::system::error_code& ec, std::size_t bytes) {
			std::cout << "Packet received: {" << in_data << "}\n";
			std::cout << "Server endpoint: " << server_endpoint << "\n";
		});
	
	// This dummy example is bad, because these local variables will go out of scope once this function
	// ends, and the socket could still be listening, but it works for this simple test
	std::this_thread::sleep_for(10s);
}
