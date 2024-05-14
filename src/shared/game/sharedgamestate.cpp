#include "shared/game/sharedgamestate.hpp"

void SharedGameState::update(const SharedGameState& other) {
    // copy over static data that is sent every update
    this->lobby = other.lobby;
    this->phase = other.phase;
    this->timestep = other.timestep;
    this->timestep_length = other.timestep_length;

    // Update our own objects hash map based on new updates
    for (const auto& [id, updated_obj] : other.objects) { // cppcheck-suppress unassignedVariable
        this->objects[id] = updated_obj;
    }
}