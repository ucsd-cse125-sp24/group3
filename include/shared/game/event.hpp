#pragma once

#include <boost/variant/variant.hpp>
#include <iostream>

#include "shared/utilities/typedefs.hpp"
#include "shared/utilities/serialize.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "shared/game/celltype.hpp"
#include "shared/audio/soundcommand.hpp"
#include "shared/utilities/constants.hpp"


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
    LoadSoundCommands,
    StartAction,
    StopAction, 
    MoveRelative,
    MoveAbsolute,
    SpawnEntity,
    SelectItem,
    UseItem,
    DropItem,
    UpdateLightSources,
    TrapPlacement,
    LoadIntroCutscene,
};

enum class ActionType {
    MoveCam,
    Jump,
    Sprint,
    Zoom
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
 *  Enum representing a player's desired role (as selected with the radio buttons
 *  in the client lobby screen (in GUIState::Lobby)
 */
enum class PlayerRole {
    Player,
    DungeonMaster,
    Unknown
};

/**
 * Event representing an action a player can take during the Lobby GUI
 * screen.
 * (Only handle this event in the server while the game phase is set to
 * GamePhase::LOBBY)
 */
struct LobbyActionEvent {
    enum class Action {
        Ready,
        StartGame
    };

    LobbyActionEvent() {}
    explicit LobbyActionEvent(Action action, PlayerRole role) : action(action),
        role(role) {}

    Action action;
    PlayerRole role;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar& action & role;
    }
};

/**
 * Event sent by the server to a client, giving a partial update to the SharedGameState
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
 * Event sent by the server to a client, giving a partial update to the AudioTable
 */
struct LoadSoundCommandsEvent {
    // Dummy value doesn't matter because will be overridden with whatever you deserialize
    LoadSoundCommandsEvent() = default;
    explicit LoadSoundCommandsEvent(const std::vector<SoundCommand>& commands) : commands(commands) {}

    std::vector<SoundCommand> commands;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & commands;
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
 * Event for placing a trap
 */
struct TrapPlacementEvent {
    TrapPlacementEvent() {}
    TrapPlacementEvent(EntityID entity_to_act, glm::vec3 world_pos, CellType cell, bool hover, bool place) : entity_to_act(entity_to_act), world_pos(world_pos), cell(cell), hover(hover), place(place) { }

    EntityID entity_to_act;
    CellType cell;
    glm::vec3 world_pos;
    bool hover, place;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar& entity_to_act & cell & world_pos & hover & place;
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
    explicit SelectItemEvent(EntityID playerEID, int itemNum) : playerEID(playerEID), itemNum(itemNum) {}

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
    explicit UseItemEvent(EntityID playerEID) : playerEID(playerEID) {}

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
    explicit DropItemEvent(EntityID playerEID) : playerEID(playerEID){}

    EntityID playerEID;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar& playerEID;
    }

};

struct UpdateLightSourcesEvent {
    struct UpdatedLightSource {
        EntityID eid;
        float intensity;
        bool is_cut;
        DEF_SERIALIZE(Archive& ar, const unsigned int version) {
            ar & eid & intensity & is_cut;
        }
    };
    std::array<boost::optional<UpdatedLightSource>, MAX_POINT_LIGHTS> lightSources;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & lightSources;
    }
};

struct LoadIntroCutsceneEvent {
    LoadIntroCutsceneEvent() = default;

    explicit LoadIntroCutsceneEvent(
        const SharedGameState& state,
        EntityID pov_eid,
        EntityID dm_eid,
        const std::array<boost::optional<SharedObject>, MAX_POINT_LIGHTS>& lights
    ) : state(state), pov_eid(pov_eid), dm_eid(dm_eid), lights(lights) {}

    SharedGameState state;
    EntityID pov_eid;
    EntityID dm_eid;
    std::array<boost::optional<SharedObject>, MAX_POINT_LIGHTS> lights;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & state & lights & pov_eid & dm_eid;
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
    LoadSoundCommandsEvent,
    StartActionEvent,
    StopActionEvent,
    MoveRelativeEvent,
    MoveAbsoluteEvent,
    SpawnEntityEvent,
    SelectItemEvent,
    UseItemEvent,
    UpdateLightSourcesEvent,
    DropItemEvent,
    TrapPlacementEvent,
    LoadIntroCutsceneEvent
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