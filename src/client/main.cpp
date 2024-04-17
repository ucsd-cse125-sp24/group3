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
        std::cerr << "Error: lobby discovery not enabled yet for client-side."
            << std::endl;
        std::exit(1);

        // lobby_finder.startSearching();
    } else {
        client.connectAndListen(config.network.server_ip);
    }

    while (true)
    {
        context.run_for(1s);

        // probably want to put rendering logic inside of client, so that this main function
        // mimics the server one where all of the important logic is done inside of a run command
        // But this is a demo of how you could use the client session to get information from
        // the game state

        for (Event event : client.client_session->getEvents()) {
            std::cout << "Event Received: " << event << std::endl;
            if (event.type == EventType::LoadGameState) {
                auto data = boost::get<LoadGameStateEvent>(event.data);
                for (const auto& [eid, player] : data.state.getLobbyPlayers()) {
                    std::cout << "\tPlayer " << eid << ": " << player << "\n";
                }
                std::cout << "\tThere are " <<
                    data.state.getLobbyMaxPlayers() - data.state.getLobbyPlayers().size() <<
                    " slots remaining in this lobby\n";
            }
        }
    }

    return 0;
}
