#include "server/game/arrowtrap.hpp"
#include "server/game/object.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/utilities/rng.hpp"
#include "server/game/objectmanager.hpp"
#include "server/game/projectile.hpp"
#include "server/game/collider.hpp"
#include "shared/audio/constants.hpp"
#include <chrono>

using namespace std::chrono_literals;

const std::chrono::seconds ArrowTrap::TIME_UNTIL_RESET = 4s;

ArrowTrap::ArrowTrap(glm::vec3 corner, glm::vec3 dimensions, Direction dir):
    Trap(ObjectType::ArrowTrap, false, corner, Collider::Box, ModelType::Cube, dimensions) 
{
    this->dir = dir;
    this->shoot_time = std::chrono::system_clock::now();
    switch (dir) {
        case ArrowTrap::Direction::LEFT:
            this->physics.shared.facing = glm::vec3(-1.0f, 0.0f, 0.0f);
            break;
        case ArrowTrap::Direction::RIGHT:
            this->physics.shared.facing = glm::vec3(1.0f, 0.0f, 0.0f);
            break;
        case ArrowTrap::Direction::UP:
            this->physics.shared.facing = glm::vec3(0.0f, 0.0f, -1.0f);
            break;
        case ArrowTrap::Direction::DOWN:
            this->physics.shared.facing = glm::vec3(0.0f, 0.0f, 1.0f);
            break;
    }
}

bool ArrowTrap::shouldTrigger(ServerGameState& state) {
    if (this->info.triggered) {
        return false;
    }

    std::vector<glm::ivec2> player_grid_positions;
    for (int i = 0; i < state.objects.getPlayers().size(); i++) {
        Player* player = state.objects.getPlayers().get(i);
        if (player == nullptr) continue;
        player_grid_positions.push_back(state.getGrid().getGridCellFromPosition(player->physics.shared.getCenterPosition()));
    }
    glm::ivec2 curr_grid_pos = state.getGrid().getGridCellFromPosition(this->physics.shared.getCenterPosition());
    int dist = 0;
    while (dist < 10) { // max sightline
        if (state.getGrid().getCell(curr_grid_pos.x, curr_grid_pos.y)->type == CellType::Wall) {
            return false; // didnt find a player before a wall
        }

        for (const auto& curr_player_pos : player_grid_positions) {
            if (curr_grid_pos == curr_player_pos) { // cppcheck-suppress useStlAlgorithm
                return randomInt(1, 4) == 1;
            }
        } 

        curr_grid_pos.x += this->physics.shared.facing.x;
        curr_grid_pos.y += this->physics.shared.facing.z;

        dist++;
    }

    return false;
}

void ArrowTrap::trigger(ServerGameState& state) {
    Trap::trigger(state);

    glm::vec3 arrow_origin(
        this->physics.shared.getCenterPosition().x,
        2.0f,
        this->physics.shared.getCenterPosition().z   
    );

    // TODO scale with grid size?
    switch (this->dir) {
        case Direction::UP:
            arrow_origin.z -= 3.0f;
            break;
        case Direction::DOWN:
            arrow_origin.z += 2.0f;
            break;
        case Direction::LEFT:
            arrow_origin.x -= 3.0f;
            break;
        case Direction::RIGHT:
            arrow_origin.x += 2.0f;
            break;
    }

    state.objects.createObject(new Arrow(arrow_origin,
        this->physics.shared.facing, this->dir));

    this->shoot_time = std::chrono::system_clock::now();

    state.soundTable().addNewSoundSource(SoundSource(
        ServerSFX::ArrowShoot,
        this->physics.shared.getCenterPosition(),
        DEFAULT_VOLUME,
        MEDIUM_DIST,
        MEDIUM_ATTEN
    ));
}

bool ArrowTrap::shouldReset(ServerGameState& state) {
    auto now = std::chrono::system_clock::now();
    return (now - this->shoot_time > TIME_UNTIL_RESET);
}

void ArrowTrap::reset(ServerGameState& state) {
    Trap::reset(state);
}
