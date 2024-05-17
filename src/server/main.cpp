#include <iostream>
#include <chrono>

#include <boost/asio/io_context.hpp>

#include "server/server.hpp"
#include "shared/utilities/rng.hpp"
#include "shared/utilities/config.hpp"

#include "server/game/mazegenerator.hpp"

using namespace std::chrono_literals;

int main(int argc, char** argv) {
    auto config = GameConfig::parse(argc, argv);
    boost::asio::io_context context;
    Server server(context, config);

    while (true) {
        // Do one tick of updates
        auto wait_time = server.doTick();

        if (wait_time <= 0ms) {
            std::cerr << "WARNING: did not meet tick rate!\n";
            context.run_for(5ms);
        } else {
            // Wait until next tick
            context.run_for(wait_time);
        }
    }
}
