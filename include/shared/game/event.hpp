#pragma once

#include <variant>

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

struct LobbyActionEvent {
    enum class Action {
        LEAVE,
        READY_UP,
        UNREADY
    };

    Action action;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & action;
    }
};

struct LoadGameStateEvent {
    GameState state;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & state;
    }
};

struct MoveRelativeEvent {
    EntityID entity_to_move;
    /// some velocity / movement information...

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {

    }
};

struct MoveAbsoluteEvent {
    EntityID entity_to_move;
    /// some velocity / movement information...

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        // TODO:
    }
};

struct SpawnEntityEvent {
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

/**
 * Struct to represent any possible event that could happen in our game. 
 */
struct Event {
    /// @brief who is attempting to trigger this event (if client -> server) or who is
    /// triggering this event (if server -> client)
    EntityID evt_source;
    /// @brief The type of event
    EventType type;
    /// @brief All of the different kinds of event data that you might have. Depending on
    /// the value of type, you should look at the data associated with that type of event.
    std::variant<
        LobbyActionEvent,
        LoadGameStateEvent,
        MoveRelativeEvent,
        MoveAbsoluteEvent,
        SpawnEntityEvent
    > data;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & evt_source & type & data;
    }
};
