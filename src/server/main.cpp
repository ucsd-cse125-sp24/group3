#include <iostream>

#include <boost/asio/io_context.hpp>

#include "server/server.hpp"
#include "shared/utilities/rng.hpp"

int main() {
    boost::asio::io_context context(1);
    Server server(context);

    while (true) {
        // server is handling broadcasting in the background,
        // next step is getting it to simulatously accept tcp connections
    }
}
