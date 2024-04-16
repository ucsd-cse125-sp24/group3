#include <iostream>
#include <chrono>

#include <boost/asio/io_context.hpp>

#include "client/client.hpp"
#include "shared/utilities/rng.hpp"
#include "shared/utilities/config.hpp"

using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
    auto config = GameConfig::parse(argc, argv);
    boost::asio::io_context context;
    LobbyFinder lobby_finder(context, config);
    Client client(context, config);
    if (config.client.lobby_discovery) {
        // TODO: once we have UI, there should be a way to connect based on
        // this. Right now, there isn't really a way to react to the information
        // the LobbyFinder is gathering.
        std::cerr << "Error: lobby discovery not implemented yet for client-side."
            << std::endl;
        std::exit(1);

        // lobby_finder.startSearching();
    } else {
        client.connectAndListen(config.network.server_ip);
    }

    while (true)
    {
        context.run_for(1s);

        for (const auto& [type, data] : client.client_session->getAllReceivedPackets()) {
            std::cout << "Recevied packet type " << static_cast<int>(type) << "\n";
            std::cout << "Recevied packet data " << data << "\n";
        }
    }

    return 0;
}
