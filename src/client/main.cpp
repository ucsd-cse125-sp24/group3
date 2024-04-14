#include <iostream>
#include <chrono>

#include <boost/asio/io_context.hpp>

#include "client/client.hpp"
#include "shared/utilities/rng.hpp"
#include "shared/utilities/config.hpp"

using namespace std::chrono_literals;

int main(int argc, char* argv[])
{
    // nlohmann::json config = parseConfig(argc, argv);

    boost::asio::io_context context;

    // LobbyFinder lobby_finder(context);
    Client client(context, "localhost");
    client.connectAndListen();

    // Send data to server
    std::string client_name = argv[1];

    while (true)
    {
        context.run_for(1s);

        auto packet = packagePacket(packet::Type::ClientDeclareInfo,
            packet::ClientDeclareInfo{ .player_name = client_name });

        client.client_session->sendPacketAsync(packet);

        for (const auto& [type, data] : client.client_session->getAllReceivedPackets()) {
            std::cout << "Recevied packet type " << static_cast<int>(type) << "\n";
            std::cout << "Recevied packet data " << data << "\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}