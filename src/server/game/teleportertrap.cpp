#include "server/game/teleportertrap.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/utilities/rng.hpp"
#include "server/game/objectmanager.hpp"
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

    while (true) {
        r_col = randomInt(0, grid.getColumns() - 1);
        r_row = randomInt(0, grid.getRows() - 1);

        if (grid.getCell(r_col, r_row)->type == CellType::Empty) {
            break;
        }
    }

    state.objects.moveObject(other, glm::vec3(r_col * grid.grid_cell_width, 0.0f, r_row * grid.grid_cell_width));
}
