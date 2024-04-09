#include "server/server.hpp"

#include <boost/asio/io_context.hpp>
#include <iostream>

Server::Server() {
	boost::asio::io_context io_context(1);
	std::cout << "test successful yipieeee!\n";
}
