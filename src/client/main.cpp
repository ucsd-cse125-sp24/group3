#include <iostream>
#include <boost/asio.hpp>

#include <thread>
#include <chrono>
#include <string>

#include "client/client.hpp"
#include "shared/utilities/config.hpp"
#include "shared/utilities/rng.hpp"

#include "shared/network/constants.hpp"

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
    // nlohmann::json config = parseConfig(argc, argv);

    boost::asio::io_context context;
    // LobbyFinder lobby_finder(context);
    Client client(context, "localhost");
    client.connectAndListen();

    while (true)
    {
        // Send data to server
        std::string client_name = argv[1];

        // std::string message = "i am client " + client_name + " and I talking to the server!";
        // std::getline(std::cin, message);

        auto packet = packagePacket(packet::Type::ClientDeclareInfo,
            packet::ClientDeclareInfo{ .player_name = client_name });

        client.client_session->sendPacketAsync(packet);
        // boost::asio::write(client.client_session->socket, packet->toBuffer());

        // replace everything below with Session->receivePacketAsync
        // Receive response from server
        //std::array<char, 128> buf;
        //boost::system::error_code error;

        //size_t len = client.client_session->socket.read_some(boost::asio::buffer(buf), error);

        //if (error == boost::asio::error::eof)
        //{
        //    std::cout << "Connection closed by server." << std::endl;
        //    break;
        //}
        //else if (error)
        //{
        //    throw boost::system::system_error(error); // Some other error.
        //}

        //std::cout << std::string(buf.begin(), buf.end()) << std::endl;

        // for (const auto& [endpoint, lobby] : lobby_finder.getFoundLobbies()) {
        //     std::cout << "------------------\n";
        //     std::cout << lobby.lobby_name << " @ " << endpoint << "\n";
        //     std::cout << lobby.slots_taken << "/" << lobby.slots_avail + lobby.slots_taken << "\n";
        //     std::cout << "------------------\n";
        //     std::cout << std::endl;
        // }

        for (const auto& [type, data] : client.client_session->getAllReceivedPackets()) {
            std::cout << "Recevied packet type " << static_cast<int>(type) << "\n";
            std::cout << "Recevied packet data " << data << "\n";
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}