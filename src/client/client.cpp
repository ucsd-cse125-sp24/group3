#include "client/client.hpp"

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <thread>

#include "shared/network/constants.hpp"
#include "shared/network/packet.hpp"
#include "shared/utilities/config.hpp"

using namespace boost::asio::ip;
using namespace std::chrono_literals;

Client::Client(boost::asio::io_context& io_context, GameConfig config):
    resolver(io_context),
    socket(io_context),
    config(config)
{
    
}

void Client::connectAndListen(std::string ip_addr) {
    this->endpoints = resolver.resolve(ip_addr, std::to_string(config.network.server_port));
    this->client_session = std::make_shared<Session>(std::move(this->socket), SessionInfo {
        .client_name = this->config.client.default_name,
        .client_eid = {}
    });

    this->client_session->connectTo(this->endpoints);

    auto packet = PackagedPacket::make_shared(packet::Type::ClientDeclareInfo,
        packet::ClientDeclareInfo { .player_name = config.client.default_name });

    this->client_session->sendPacketAsync(packet);

    this->client_session->startListen();
}
