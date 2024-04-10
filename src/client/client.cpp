#include "client/client.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <thread>
#include <sstream>

#include "shared/network/constants.hpp"
#include "shared/network/packet.hpp"


using namespace boost::asio::ip;
using namespace std::chrono_literals;

Client::Client(boost::asio::io_context& io_context)
    :lobby_discovery_socket(io_context, udp::endpoint(address_v4::any(), PORT))
{
    this->lobby_discovery_socket.async_receive_from(
        boost::asio::buffer(this->lobby_info_buf), this->endpoint_buf,
        [this](const boost::system::error_code& ec, std::size_t bytes) {
            std::string packet(this->lobby_info_buf.begin(), this->lobby_info_buf.end());
            std::cout << bytes << "\n";
            std::cout << packet.size() << "\n";
            packet.resize(bytes);
            std::cout << "packet: " << packet << "\n";
            std::cout << packet.size() << "\n";
            packet::ServerLobbyBroadcast parsed_info;
            std::istringstream stream(packet);
            boost::archive::text_iarchive archive(stream);
            archive >> parsed_info;

            std::cout << "Packet received\n";
            std::cout << parsed_info.lobby_name << "\n";
            std::cout << parsed_info.slots_avail << "\n";
            std::cout << parsed_info.slots_taken << "\n";
            std::cout << "Server endpoint: " << this->endpoint_buf << "\n";
        });
}
