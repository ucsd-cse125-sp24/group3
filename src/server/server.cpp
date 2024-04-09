#include "server/server.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <iostream>
#include <fstream>

#include "shared/network/packet.hpp"

Server::Server() {
	boost::asio::io_context io_context(1);

	std::cout << "test successful yipieeee!\n";
}
