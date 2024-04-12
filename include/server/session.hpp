#pragma once

#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <queue>
#include <mutex>
#include <thread>

#include "shared/network/packet.hpp"
#include "shared/network/gamesocket.hpp"

using boost::asio::ip::tcp;

class Session
{
public:
    Session(GameSocket gsocket, EntityID eid);
    ~Session();

private:
    GameSocket gsocket;

    /// @brief eid of the player that this session is associated with.
    EntityID eid;
};