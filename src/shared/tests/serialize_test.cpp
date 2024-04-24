#include <gtest/gtest.h>

#include "shared/utilities/serialize_macro.hpp"
#include "shared/network/packet.hpp"
#include "shared/game/event.hpp"
#include "server/game/servergamestate.hpp"

TEST(SerializeTest, SerializeEvent) {
    ServerGameState state(GamePhase::LOBBY, GameConfig{});
    state.addPlayerToLobby(1, "Player Name");
    Event evt(0, EventType::LoadGameState, LoadGameStateEvent(state.generateSharedGameState()));
    Event evt2 = deserialize<Event>(serialize(evt));

    ASSERT_EQ(evt.type, EventType::LoadGameState);
    ASSERT_EQ(evt.type, evt2.type);
    ASSERT_EQ(evt.evt_source, evt2.evt_source);
    ASSERT_EQ(evt.data.which(), evt2.data.which());
    ASSERT_EQ(boost::get<LoadGameStateEvent>(evt.data).state.lobby.players.size(),
              boost::get<LoadGameStateEvent>(evt2.data).state.lobby.players.size());
}

TEST(SerializeTest, SerializePacketEvent) {
    ServerGameState state(GamePhase::LOBBY, GameConfig{});
    state.addPlayerToLobby(1, "Player Name");
    Event evt(0, EventType::LoadGameState, LoadGameStateEvent(state.generateSharedGameState()));

    EventPacket packet {.event=evt};
    EventPacket packet2 = deserialize<EventPacket>(serialize(packet));

    ASSERT_EQ(packet.event.type, EventType::LoadGameState);
    ASSERT_EQ(packet.event.type, packet2.event.type);
}