#pragma once

#include <boost/variant/variant.hpp>
#include <iostream>

#include "shared/utilities/typedefs.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "shared/game/gamestate.hpp"


// TODO: doxygen

enum class EventType {
    LobbyAction,
    LoadGameState,
    MoveRelative,
    MoveAbsolute,
    SpawnEntity,
};

std::ostream& operator<<(std::ostream& os, const EventType& type);

struct LobbyActionEvent {
    enum class Action {
        LEAVE,
        READY_UP,
        UNREADY
    };

    LobbyActionEvent() {}
    LobbyActionEvent(Action action) : action(action) {}

    Action action;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & action;
    }
};

struct LoadGameStateEvent {
    // Dummy value doesn't matter because will be overridden with whatever you deserialize
    LoadGameStateEvent() : state(GameState(GamePhase::TITLE_SCREEN, GameConfig{})){}
    LoadGameStateEvent(GameState state) : state(state) {}

    GameState state;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & state;
    }
};

struct MoveRelativeEvent {
    MoveRelativeEvent() {}
    MoveRelativeEvent(EntityID entity_to_move) : entity_to_move(entity_to_move) {}

    EntityID entity_to_move;
    /// some velocity / movement information...

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {

    }
};

struct MoveAbsoluteEvent {
    MoveAbsoluteEvent() {}
    MoveAbsoluteEvent(EntityID entity_to_move) : entity_to_move(entity_to_move) {}

    EntityID entity_to_move;
    /// some velocity / movement information...

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        // TODO:
    }
};

struct SpawnEntityEvent {
    SpawnEntityEvent() {}
    SpawnEntityEvent(EntityID new_id, int entity_type) : new_id(new_id), entity_type(entity_type) {}

    /// @brief Id of the new entity, if the server is sending this down. Ignored, if this is a client request.
    EntityID new_id;
    /// @brief What kind of entity to spawn
    int entity_type; // replace with enum

    // location information about the spawning...
    // maybe literally embed an Object here? idk

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        // TODO:
    }

};

using EventData = boost::variant<
    LobbyActionEvent,
    LoadGameStateEvent,
    MoveRelativeEvent,
    MoveAbsoluteEvent,
    SpawnEntityEvent
>;

/**
 * Struct to represent any possible event that could happen in our game. 
 */
struct Event {
    Event() {}
    Event(EntityID evt_source, EventType type, EventData data)
        :evt_source(evt_source), type(type), data(data) {}

    /// @brief who is attempting to trigger this event (if client -> server) or who is
    /// triggering this event (if server -> client)
    EntityID evt_source;
    /// @brief The type of event
    EventType type;
    /// @brief All of the different kinds of event data that you might have. Depending on
    /// the value of type, you should look at the data associated with that type of event.
    EventData data;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & evt_source & type & data;
    }
};

std::ostream& operator<<(std::ostream& os, const Event& evt);