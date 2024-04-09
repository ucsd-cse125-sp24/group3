#include <boost/asio/io_context.hpp>

#include <iostream>

int test() {
	boost::asio::io_context io_context(1);
	std::cout << "test successful\n";
	return 0;
}