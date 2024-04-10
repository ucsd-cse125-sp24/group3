#include <iostream>

#include "client/client.hpp"
#include "shared/utilities/rng.hpp"

using namespace std::chrono_literals;

int main() {
    boost::asio::io_context context(1);
    LobbyFinder lobby_finder(context);
    Client client(context);

    while (true) {
        for (const auto& [endpoint, lobby] : lobby_finder.getFoundLobbies()) {
            std::cout << "------------------\n";
            std::cout << lobby.lobby_name << " @ " << endpoint << "\n";
            std::cout << lobby.slots_taken << "/" << lobby.slots_avail + lobby.slots_taken << "\n";
            std::cout << "------------------\n";
            std::cout << std::endl;
        }

        std::this_thread::sleep_for(1s);
    }

    while (1);
}
