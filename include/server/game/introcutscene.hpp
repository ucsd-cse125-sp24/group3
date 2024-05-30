#pragma once

#include "server/game/servergamestate.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "shared/game/sharedgamestate.hpp"

#include <vector>

/**
 * @brief config to use for the servergamestate used for the cutscene,
 * This mainly just needs to set the room file to load the cutscene room
 * from. I dont think the other values will really matter.
 */
GameConfig getCutsceneConfig();

class IntroCutscene {
public:
    IntroCutscene();

    /**
     * update to the next frame of the cutscene
     * @returns true if the cutscene is over
     */
    bool update();

    LoadIntroCutsceneEvent toNetwork();

    ServerGameState state;
private:
    std::array<boost::optional<SharedObject>, MAX_POINT_LIGHTS> lights;
    EntityID pov_eid;
};
