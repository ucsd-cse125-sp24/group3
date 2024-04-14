#include "client/client.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <thread>

#include "shared/network/constants.hpp"
#include "shared/network/packet.hpp"


using namespace boost::asio::ip;
using namespace std::chrono_literals;

Client::Client(boost::asio::io_context& io_context, std::string ip_addr):
    resolver(io_context),
    socket(io_context)
{
    this->endpoints = resolver.resolve(ip_addr, std::to_string(PORT));
}

void Client::connect() {
    boost::asio::connect(this->socket, this->endpoints);
}
