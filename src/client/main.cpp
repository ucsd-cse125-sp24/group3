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
    try
    {
        // nlohmann::json config = parseConfig(argc, argv);

        boost::asio::io_context context;
        LobbyFinder lobby_finder(context);
        Client client(context);

        // Resolve the server address
        tcp::resolver resolver(context);
        auto endpoints = resolver.resolve("localhost", "45623");

        // Create and connect socket
        tcp::socket socket(context);
        boost::asio::connect(socket, endpoints);

        while (true)
        {
            // Send data to server
            std::string client_name = argv[1];

            std::string message = "i am client " + client_name + " and I talking to the server!";
            // std::getline(std::cin, message);

            boost::asio::write(socket, boost::asio::buffer(message + "\n"));

            // Receive response from server
            std::array<char, 128> buf;
            boost::system::error_code error;
            size_t len = socket.read_some(boost::asio::buffer(buf), error);

            if (error == boost::asio::error::eof)
            {
                std::cout << "Connection closed by server." << std::endl;
                break;
            }
            else if (error)
            {
                throw boost::system::system_error(error); // Some other error.
            }

            std::cout.write(buf.data(), len);
            std::cout << std::endl;

            for (const auto& [endpoint, lobby] : lobby_finder.getFoundLobbies()) {
                std::cout << "------------------\n";
                std::cout << lobby.lobby_name << " @ " << endpoint << "\n";
                std::cout << lobby.slots_taken << "/" << lobby.slots_avail + lobby.slots_taken << "\n";
                std::cout << "------------------\n";
                std::cout << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 0;
}
