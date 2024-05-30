#include "server/audio/soundtable.hpp"
#include "shared/game/constants.hpp"
#include "shared/utilities/smartvector.hpp"
#include "server/game/player.hpp"

#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

SoundTable::SoundTable() {
    this->next_id = 0;
}

std::unordered_map<EntityID, std::vector<SoundCommand>> SoundTable::getCommandsPerPlayer(const std::vector<Object*>& players) {
    std::vector<SoundCommand> commands;
    std::swap(this->current_commands, commands);
    // current commands is now empty, and we can parse these commands as we wish

    std::unordered_map<EntityID, std::vector<SoundCommand>> commands_per_player;

    for (const auto& obj : players) {
        for (const SoundCommand& command : commands) {
            if (command.source.canBeHeardFrom(obj->physics.shared.getCenterPosition())) {
                commands_per_player[obj->globalID].push_back(command);
            }
        }
    }

    return commands_per_player;
}

void SoundTable::tickSounds() {
    for (auto it = this->map.begin(); it != this->map.end(); ) {
        auto& [id, source] = *it; // cppcheck-suppress variableScope

        if (source.SERVER_time_remaining.has_value()) {
            source.SERVER_time_remaining.value() -= TIMESTEP_LEN;
            if (source.SERVER_time_remaining.value() <= 0ms) {
                // push back BEFORE erase so the ref to id and source don't go out of scope!!
                this->current_commands.push_back(SoundCommand(id, SoundAction::DO_DELETE, source));
                it = this->map.erase(it);
                continue;
            }
        }

        ++it;
    }
}

void SoundTable::addStaticSoundSource(const SoundSource& source) {
    SoundID id = this->next_id++;
    this->map.insert({id, source});
    this->static_sources.push_back(id);
}

void SoundTable::addNewSoundSource(const SoundSource& source) {
    SoundID id = this->next_id++;
    this->map.insert({id, source});
    this->current_commands.push_back(SoundCommand(id, SoundAction::PLAY, source));
}

const std::unordered_map<SoundID, SoundSource>& SoundTable::data() const {
    return this->map;
}