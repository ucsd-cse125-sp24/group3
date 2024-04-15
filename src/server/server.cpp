#include "server/server.hpp"

#include <boost/asio/io_context.hpp>
#include <boost/asio/ip/udp.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <iostream>
#include <fstream>
#include <ostream>
#include <thread>

#include "shared/network/session.hpp"
#include "shared/network/packet.hpp"
#include "shared/network/constants.hpp"
#include "shared/utilities/config.hpp"

using namespace std::chrono_literals;
using namespace boost::asio::ip;

Server::Server(boost::asio::io_context& io_context, GameConfig config)
    :lobby_broadcaster(io_context, config),
     acceptor(io_context, tcp::endpoint(tcp::v4(), config.network.server_port)),
     socket(io_context)
{
    doAccept(); // start asynchronously accepting

    if (config.server.lobby_broadcast) {
        this->lobby_broadcaster.startBroadcasting(packet::ServerLobbyBroadcast {
            .lobby_name  = config.server.lobby_name,
            .slots_taken = 0,
            .slots_avail = config.server.max_players});
    }
}

EntityID Server::genNewEID() {
    static EntityID id = 0;
    return id++;
}

void Server::doAccept() {
    this->acceptor.async_accept(this->socket,
        [this](boost::system::error_code ec) {
            if (!ec) {
                EntityID eid = Server::genNewEID();
                auto session = std::make_shared<Session>(std::move(this->socket));

                session->startListen();

                this->sessions.insert({eid, session});

                session->sendPacketAsync(packagePacket(packet::Type::ServerAssignEID,
                    packet::ServerAssignEID{ .eid = eid }));
            } else {
                std::cerr << "Error accepting tcp connection: " << ec << std::endl;
            }

            doAccept();
        });
}

const std::unordered_map<EntityID, std::shared_ptr<Session>>& Server::getSessions() {
    return this->sessions;
}