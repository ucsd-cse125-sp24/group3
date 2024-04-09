#include "client/client.hpp"

#include <boost/asio/io_context.hpp>
#include <iostream>

Client::Client() {
	boost::asio::io_context io_context(1);
	std::cout << "test successful yipieeee!\n";
}
