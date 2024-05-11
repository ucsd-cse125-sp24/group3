#pragma once

#include <boost/variant/variant.hpp>
#include <iostream>

#include "shared/utilities/typedefs.hpp"
#include "shared/utilities/serialize.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "shared/game/sharedgamestate.hpp"


/****************************************************
 *
 * Important notes for adding new events:
 *
 * 1. Make sure you correctly define the serialization function for all data members
 * 2. Make sure you define two constructors: one "dummy" default constructor which
 *    does nothing, and one constructor to actually make the Events. The dummy constructor
 *    is needed to make everything compile.
 * 3. Make sure you add the new struct you make to the EventData boost::variant typedef
 *    further down.
 *
 ***************************************************/

 /**
  * Tag for the different kind of events there are
  */
enum class EventType {
    ChangeFacing,
    LobbyAction,
    LoadGameState,
    StartAction,
    StopAction, 
    MoveRelative,
    MoveAbsolute,
    SpawnEntity,
    SelectItem,
    UseItem,
    DropItem,
};

enum class ActionType {
    MoveCam,
    Jump,
    Sprint,
};

/**
 * Override << so we can std::cout << EventType_var;
 */
std::ostream& operator<<(std::ostream& os, const EventType& type);

/**
 * Event for an entity changing their facing direction
 */
struct ChangeFacingEvent {
    ChangeFacingEvent() {}
    ChangeFacingEvent(EntityID entity_to_change_face, glm::vec3 facing) : entity_to_change_face(entity_to_change_face), facing(facing) { }

    glm::vec3 facing;
    EntityID entity_to_change_face;
    /// some velocity / movement information...

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & entity_to_change_face & facing;
    }
};

/**
 * Event representing an action a player can take during the lobby screen
 */
struct LobbyActionEvent {
    enum class Action {
        LEAVE,
        READY_UP,
        UNREADY
    };

    LobbyActionEvent() {}
    explicit LobbyActionEvent(Action action) : action(action) {}

    Action action;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar& action;
    }
};

/**
 * Event sent by the server to a client, telling the client to update their SharedGameState
 * to this new SharedGameState
 */
struct LoadGameStateEvent {
    // Dummy value doesn't matter because will be overridden with whatever you deserialize
    LoadGameStateEvent() : state(SharedGameState(GamePhase::TITLE_SCREEN, GameConfig{})) {}
    explicit LoadGameStateEvent(const SharedGameState& state) : state(state) {}

    SharedGameState state;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar& state;
    }
};

/**
 * Event for action to start for generic key pressed / Can be updated to action type enum later
 */
struct StartActionEvent {
    StartActionEvent() {}
    StartActionEvent(EntityID entity_to_act, glm::vec3 movement, ActionType action) : entity_to_act(entity_to_act), movement(movement), action(action) { }

    EntityID entity_to_act;
    glm::vec3 movement;
    ActionType action;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar& entity_to_act& movement& action;
    }
};

/**
 * Event for action to stop for generic key pressed
 */
struct StopActionEvent {
    StopActionEvent() {}
    StopActionEvent(EntityID entity_to_act, glm::vec3 movement, ActionType action) : entity_to_act(entity_to_act), movement(movement), action(action) { }

    EntityID entity_to_act;
    glm::vec3 movement;
    ActionType action;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar& entity_to_act& movement& action;
    }
};

/**
 * Event for an entity moving a relative distance
 */
struct MoveRelativeEvent {
    MoveRelativeEvent() {}
    MoveRelativeEvent(EntityID entity_to_move, glm::vec3 movement) : entity_to_move(entity_to_move), movement(movement) { }

    glm::vec3 movement;
    EntityID entity_to_move;
    /// some velocity / movement information...

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & entity_to_move & movement;
    }
};

/**
 * Event for an entity moving to some new absolute location
 */
struct MoveAbsoluteEvent {
    MoveAbsoluteEvent() {}
    explicit MoveAbsoluteEvent(EntityID entity_to_move) : entity_to_move(entity_to_move) {}

    EntityID entity_to_move;
    /// some velocity / movement information...

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        // TODO:
    }
};

/**
 * Event for the creation of a new Entity
 */
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

/**
 * Event for selecting which item to use 
 */
struct SelectItemEvent {
    SelectItemEvent() {}
    SelectItemEvent(EntityID playerEID, int itemNum) : playerEID(playerEID), itemNum(itemNum) {}

    EntityID playerEID;
    int itemNum;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar& playerEID& itemNum;
    }

};

/**
 * Event for entity to use item
 */
struct UseItemEvent {
    UseItemEvent() {}
    UseItemEvent(EntityID playerEID) : playerEID(playerEID) {}

    EntityID playerEID;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar& playerEID;
    }
};

/**
 * Event for dropping item in inventory
 */
struct DropItemEvent {
    DropItemEvent() {}
    DropItemEvent(EntityID playerEID) : playerEID(playerEID){}

    EntityID playerEID;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar& playerEID;
    }

};

/**
 * All of the different kinds of events in a tagged union, so we can
 * easily pull out the actual data for a specific Event
 */
using EventData = boost::variant<
    ChangeFacingEvent,
    LobbyActionEvent,
    LoadGameStateEvent,
    StartActionEvent,
    StopActionEvent,
    MoveRelativeEvent,
    MoveAbsoluteEvent,
    SpawnEntityEvent,
    SelectItemEvent,
    UseItemEvent,
    DropItemEvent
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
    /// @brief The type of event. This actually might not be needed because I think boost::variant
    /// has a tag built in, but We're already using it everywhere...
    EventType type;
    /// @brief All of the different kinds of event data that you might have. Depending on
    /// the value of type, you should look at the data associated with that type of event.
    EventData data;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar& evt_source& type& data;
    }
};

/**
 * Allow us to std::cout an Event
 *
 * TODO: actually output the data for the EventData
 */
std::ostream& operator<<(std::ostream& os, const Event& evt);