#include "shared/audio/soundtable.hpp"
#include "shared/game/constants.hpp"

#include <chrono>
#include <iostream>

using namespace std::chrono_literals;

SoundTable::SoundTable() {
    this->SERVER_next_id = 0;
}

void SoundTable::tickSounds() {
    for (auto it = this->map.begin(); it != this->map.end(); ++it) {
        auto& [id, source] = *it;
        if (!source.has_value()) {
            continue;
        }

        source->SERVER_time_remaining -= TIMESTEP_LEN;
        if (source->SERVER_time_remaining <= 0ms) {
            this->map.at(id) = {};
            this->id_free_list.push_back(id);
        }
    }
}

void SoundTable::addNewSoundSource(SoundSource source) {
    if (this->id_free_list.empty()) {
        this->map.insert({this->SERVER_next_id++, source});
    } else {
        auto id = this->id_free_list.front();
        this->id_free_list.pop_front();
        this->map.insert({id, source});
    }
}

void SoundTable::updateSoundSource(SoundID id, boost::optional<SoundSource> source) {
    this->map[id] = source;
}

const std::unordered_map<SoundID, boost::optional<SoundSource>> SoundTable::data() const {
    return this->map;
}