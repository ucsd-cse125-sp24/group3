#include <iostream>
#include <chrono>

#include <boost/asio/io_context.hpp>

#include "server/server.hpp"
#include "shared/utilities/rng.hpp"
#include "shared/utilities/config.hpp"

using namespace std::chrono_literals;

int main(int argc, char** argv) {
    auto config = GameConfig::parse(argc, argv);
    boost::asio::io_context context;
    Server server(context, config);

    while (true) {
        context.run_for(1s);

        for (const auto& [eid, session] : server.getSessions()) {
            std::cout << "Session " << eid << "\n";
            for (const auto& [type, data] : session->getAllReceivedPackets()) {
                std::cout << "Recevied packet type " << static_cast<int>(type) << "\n";
                std::cout << "Recevied packet data " << data << "\n";
            }
        }
    }
}
