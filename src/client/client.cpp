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

Client::Client(boost::asio::io_context& io_context)
{
}

void Client::connect(tcp::endpoint server) {

}
