#include "server/server.hpp"
#include "server/session.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <iostream>
#include <fstream>
#include <ostream>
#include <thread>

#include "shared/network/packet.hpp"
#include "shared/network/constants.hpp"

using namespace std::chrono_literals;
using namespace boost::asio::ip;

Server::Server(boost::asio::io_context& io_context)
    :lobby_broadcaster(io_context, // TODO: put in actual lobby info here?
        packet::ServerLobbyBroadcast {.lobby_name="My Lobby", .slots_taken=0, .slots_avail=4}),
     acceptor(io_context, tcp::endpoint(tcp::v4(), PORT)),
     socket(std::make_shared<tcp::socket>(io_context))
{
    do_accept(); // start asynchronously accepting
}

EntityID Server::genNewEID() {
    static EntityID id = 0;
    return id++;
}

void Server::do_accept() {
    this->acceptor.async_accept(*this->socket, std::bind(&Server::_handleAccept, 
        this, std::placeholders::_1, this->socket));
}

void Server::_handleAccept(boost::system::error_code ec, std::shared_ptr<tcp::socket> socket) {
    if (!ec) {
        EntityID eid = Server::genNewEID();
        GameSocket gsocket(this->socket, eid);
        auto session = std::make_shared<Session>(gsocket, eid);
        this->sessions.insert({eid, session});
    } else {
        std::cerr << "Error accepting tcp connection: " << ec << std::endl;
    }

    do_accept();
}