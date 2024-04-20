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
        // Do one tick of updates
        auto wait_time = server.doTick();

        // Wait until next tick
        context.run_for(wait_time);

        // TODO: context.run_for will return early if there is no more work to do
        // so we should figure out a way to make it always run for a specified amount
        // of time, whether we do this manually or through some API call I don't know about
        std::this_thread::sleep_for(1s); // limiting tick rate for easier to parse tests for now
    }
}
