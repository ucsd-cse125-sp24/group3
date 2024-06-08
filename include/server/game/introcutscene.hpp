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

    inline static const int START_TICK = 1;
    inline static const int STOP_MOVING_TICK = START_TICK + 250;
    inline static const int GATE_RAISE_TICK = STOP_MOVING_TICK + 30;
    inline static const int GATE_STOP_RAISE_TICK = GATE_RAISE_TICK + 200;
    inline static const int LIGHTNING_1_TICK = GATE_STOP_RAISE_TICK + 80;
    inline static const int LIGHTNING_2_TICK = LIGHTNING_1_TICK + 50;
    inline static const int LIGHTNING_3_TICK = LIGHTNING_2_TICK + 40;
    inline static const int START_PLAYER_THEME_TICK = LIGHTNING_3_TICK + 110;
    inline static const int EXIT_CUTSCENE_TICK = START_PLAYER_THEME_TICK + 240;

    int ticks;

    // just making everything public bc lazy
    ServerGameState state;
    EntityID pov_eid;
    EntityID dm_eid;
    std::array<boost::optional<SharedObject>, MAX_POINT_LIGHTS> lights;
};
