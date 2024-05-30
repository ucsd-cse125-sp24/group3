#include "server/game/introcutscene.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "shared/utilities/config.hpp"

#include <vector>


GameConfig getCutsceneConfig() {
    GameConfig config = getDefaultConfig();
    config.game.maze.maze_file = "cutscene/intro.maze";
    config.game.maze.procedural = false;
    return config;
}

IntroCutscene::IntroCutscene():
    state(GamePhase::INTRO_CUTSCENE, getCutsceneConfig())
{
    // state has loaded in the maze file we use for this cutscene,

    // hard code direction to face right based on the intro cutscene maze orientation
    Player* player = new Player(this->state.getGrid().getRandomSpawnPoint(), directionToFacing(Direction::RIGHT));
    this->state.objects.createObject(player);
    this->pov_eid = player->globalID;
    player->physics.velocity = glm::normalize(player->physics.shared.facing);

    std::size_t lights_idx = 0;
    for (int i = 0; i < this->state.objects.getObjects().size(); i++) {
        auto object = this->state.objects.getObject(i);
        if (object == nullptr) continue;

        if (lights_idx < MAX_POINT_LIGHTS && object->type == ObjectType::Torchlight) {
            this->lights[lights_idx] = object->toShared();
            lights_idx++;
        }
        if (lights_idx >= MAX_POINT_LIGHTS) {
            std::cerr << "WARNING: can't fit all lights in intro cutscene world.\n";
        }
    }

    for (int i = lights_idx; i < MAX_POINT_LIGHTS; i++) {
        this->lights[i] = {}; // make sure this is initialized, idk if this is needed but just in case
        // because i dont trust c++ to do anything ever
    }
}

bool IntroCutscene::update() {
    this->state.updateMovement();
    this->state.doTorchlightTicks();
    
    static int ticks = 0;
    ticks++;
    return (ticks > 100); // stop after 100 ticks, hard coded for now
}

LoadIntroCutsceneEvent IntroCutscene::toNetwork() {
    std::vector<SharedGameState> updates = this->state.generateSharedGameState(true);
    if (updates.size() != 1) {
        std::cerr << "FATAL: Cutscene state can't fit in one packet... Would have to refactor to make this work.\n";
        std::exit(1);
    }


    auto evt = LoadIntroCutsceneEvent(updates.at(0), this->pov_eid, this->lights);

    return evt;
}
