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
    // move half a grid cell down to be in center of 4 wide hall
    player->physics.shared.corner += directionToFacing(Direction::DOWN) * (Grid::grid_cell_width / 2.0f);

    DungeonMaster* dm = new DungeonMaster(player->physics.shared.corner + glm::vec3(-20.0f, 10.0f, 0), directionToFacing(Direction::RIGHT));
    dm->physics.velocity = player->physics.velocity;
    dm->physics.velocityMultiplier = player->physics.velocityMultiplier;
    this->state.objects.createObject(dm);
    this->dm_eid = dm->globalID;

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
    this->state.updateAttacks();

    static int ticks = 0;
    ticks++;

    Player* player = this->state.objects.getPlayer(0);
    DungeonMaster* dm = this->state.objects.getDM();

    const int START_TICK = 1;
    const int STOP_MOVING_TICK = START_TICK + 250;
    const int GATE_RAISE_TICK = STOP_MOVING_TICK + 30;
    const int GATE_STOP_RAISE_TICK = GATE_RAISE_TICK + 200;
    const int LIGHTNING_1_TICK = GATE_STOP_RAISE_TICK + 80;
    const int LIGHTNING_2_TICK = LIGHTNING_1_TICK + 50;
    const int LIGHTNING_3_TICK = LIGHTNING_2_TICK + 40;
    const int START_PLAYER_THEME_TICK = LIGHTNING_3_TICK + 110;
    const int EXIT_CUTSCENE_TICK = START_PLAYER_THEME_TICK + 240;

    if (ticks == START_TICK) {
        this->state.soundTable().addNewSoundSource(SoundSource(
            ServerSFX::TorchLoop, 
            player->physics.shared.getCenterPosition(),
            FULL_VOLUME,
            MEDIUM_DIST,
            MEDIUM_ATTEN 
        ));
        this->state.soundTable().addNewSoundSource(SoundSource(
            ServerSFX::Wind, 
            dm->physics.shared.getCenterPosition(),
            FULL_VOLUME,
            MEDIUM_DIST,
            MEDIUM_ATTEN 
        ));
    }

    if (ticks == STOP_MOVING_TICK) {
        player->physics.velocity = glm::vec3(0.0f);
        dm->physics.velocity = glm::vec3(0.0f);
    }

    if (ticks >= GATE_RAISE_TICK && ticks <= GATE_STOP_RAISE_TICK) {
        bool played_sound = false;

        auto walls = this->state.objects.getSolidSurfaces();
        for (int i = 0; i < walls.size(); i++) {
            auto wall = walls.get(i);
            if (wall == nullptr || wall->shared.surfaceType != SurfaceType::Pillar) continue;

            wall->physics.shared.corner.y += 0.042f;

            if (!played_sound && ticks == GATE_RAISE_TICK) {
                this->state.soundTable().addNewSoundSource(SoundSource(
                    ServerSFX::IntroGateOpen,
                    wall->physics.shared.getCenterPosition(),
                    QUIET_VOLUME, // the sound effect is already so fucking loud geez
                    FAR_DIST,
                    FAR_ATTEN
                ));
            }
        }
    }

    glm::vec3 lightning_pos1 = player->physics.shared.corner + glm::normalize(player->physics.shared.facing) * 10.0f;

    if (ticks == LIGHTNING_1_TICK) {
        this->state.soundTable().addNewSoundSource(SoundSource(
            ServerSFX::ZeusStartTheme, 
            player->physics.shared.getCenterPosition(),
            FULL_VOLUME,
            FAR_DIST,
            FAR_ATTEN
        ));
        this->state.objects.createObject(new Lightning(lightning_pos1, player->physics.shared.facing));
    }

    if (ticks == LIGHTNING_2_TICK) {
        glm::vec3 lightning_pos2 = lightning_pos1 + glm::vec3(3.0f, 0.0f, -3.0f);
        this->state.objects.createObject(new Lightning(lightning_pos2, player->physics.shared.facing));
    }

    if (ticks == LIGHTNING_3_TICK) {
        glm::vec3 lightning_pos3 = player->physics.shared.corner + glm::normalize(player->physics.shared.facing) * 4.0f;
        this->state.objects.createObject(new Lightning(lightning_pos3, player->physics.shared.facing));
    }

    if (ticks == START_PLAYER_THEME_TICK) {
        this->state.soundTable().addNewSoundSource(SoundSource(
            ServerSFX::PlayersStartTheme, 
            player->physics.shared.getCenterPosition(),
            FULL_VOLUME,
            FAR_DIST,
            FAR_ATTEN
        ));
    }

    if (ticks == EXIT_CUTSCENE_TICK) {
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


    auto evt = LoadIntroCutsceneEvent(updates.at(0), this->pov_eid, this->dm_eid, this->lights);

    return evt;
}
