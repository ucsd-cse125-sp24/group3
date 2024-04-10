#include <iostream>

#include "client/client.hpp"
#include "shared/utilities/rng.hpp"

int main() {
    boost::asio::io_context context(1);
    Client client(context);
}
