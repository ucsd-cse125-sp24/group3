#include "server/game/teleportertrap.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/utilities/rng.hpp"
#include "server/game/objectmanager.hpp"
#include "shared/audio/constants.hpp"
#include "server/game/exit.hpp"
#include <chrono>

using namespace std::chrono_literals;

TeleporterTrap::TeleporterTrap(glm::vec3 corner):
    Trap(ObjectType::TeleporterTrap, false, corner, Collider::Box, ModelType::Cube, glm::vec3(1, 4, 1)) 
{
}

bool TeleporterTrap::shouldTrigger(ServerGameState& state) {
    return false;
}

void TeleporterTrap::trigger(ServerGameState& state) {
    Trap::trigger(state);
}

bool TeleporterTrap::shouldReset(ServerGameState& state) {
    return false;
}

void TeleporterTrap::reset(ServerGameState& state) {
    Trap::reset(state);
}

void TeleporterTrap::doCollision(Object* other, ServerGameState& state) {
    if (this->info.dm_hover) {
        return;
    }

    int r_col = 0;
    int r_row = 0;
    auto& grid = state.getGrid();

    std::optional<glm::vec3> exit_pos;

    auto exits = state.objects.getExits();
    for (int i = 0; i < exits.size(); i++) {
        auto exit = exits.get(i);
        if (exit == nullptr) continue;

        exit_pos = exit->physics.shared.getCenterPosition();
        break;
    }

    int attempts = 0;
    bool good = false;

    while (true) {
        attempts++;
        r_col = randomInt(0, grid.getColumns() - 1);
        r_row = randomInt(0, grid.getRows() - 1);

        if (attempts < 10 && exit_pos.has_value() && glm::distance(glm::vec2(Grid::getGridCellFromPosition(exit_pos.value())), glm::vec2(r_col, r_row)) < 20.0f) {
            continue;
        }

        if (grid.getCell(r_col, r_row)->type == CellType::Empty) {
            good = true;
            break;
        }
    }

    state.objects.moveObject(other, glm::vec3(r_col * grid.grid_cell_width, 0.0f, r_row * grid.grid_cell_width));

    state.soundTable().addNewSoundSource(SoundSource(
        ServerSFX::Teleport,
        other->physics.shared.getCenterPosition(),
        DEFAULT_VOLUME,
        SHORT_DIST,
        SHORT_ATTEN
    ));
}
