#include "server/game/introcutscene.hpp"
#include "shared/game/sharedgamestate.hpp"
#include "shared/utilities/config.hpp"
#include "server/game/weaponcollider.hpp"
#include "shared/audio/constants.hpp"

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
    player->physics.velocity = glm::normalize(player->physics.shared.facing) * 0.10f;

    std::size_t lights_idx = 0;
    for (int i = 0; i < this->state.objects.getObjects().size(); i++) {
        auto object = this->state.objects.getObject(i);
        if (object == nullptr) continue;

        if (lights_idx < MAX_POINT_LIGHTS && object->type == ObjectType::Torchlight) {
            this->lights[lights_idx] = object->toShared();
            lights_idx++;
        }
        if (lights_idx > MAX_POINT_LIGHTS) {
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
    this->state.updateItems();
    this->state.deleteEntities();

    static int ticks = 0;
    ticks++;

    Player* player = this->state.objects.getPlayer(0);

    if (ticks == 200) {
        player->physics.velocity = glm::vec3(0.0f);
    }

    if (ticks >= 220 && ticks <= 400) {
        auto walls = this->state.objects.getSolidSurfaces();
        for (int i = 0; i < walls.size(); i++) {
            auto wall = walls.get(i);
            if (wall == nullptr || wall->shared.surfaceType != SurfaceType::Pillar) continue;

            wall->physics.shared.corner.y += 0.040f;
        }
    }

    glm::vec3 lightning_pos1 = player->physics.shared.corner + glm::normalize(player->physics.shared.facing) * 10.0f;

    if (ticks == 100) {

        this->state.objects.createObject(new Lightning(lightning_pos1, player->physics.shared.facing));
    }

    // if (ticks == 560) {
    //     glm::vec3 lightning_pos2 = lightning_pos1 + glm::vec3(3.0f, 0.0f, -3.0f);

    //     this->state.objects.createObject(new Lightning(lightning_pos2, player->physics.shared.facing));
    // }

    // if (ticks == 600) {
    //     glm::vec3 lightning_pos3 = lightning_pos1 + glm::vec3(-3.0f, 0.0f, 6.0f);

    //     this->state.objects.createObject(new Lightning(lightning_pos3, player->physics.shared.facing));
    // }

    this->state.updateAttacks();

    if (ticks == 800) {
        this->state.soundTable().addNewSoundSource(SoundSource(
            ServerSFX::PlayersStartTheme, 
            player->physics.shared.getCenterPosition(),
            FULL_VOLUME,
            FAR_DIST,
            FAR_ATTEN
        ));
    }

    if (ticks == 970) {
        return true;
    }


    return false;
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
