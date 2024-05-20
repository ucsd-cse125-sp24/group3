#include "shared/audio/soundtable.hpp"

SoundTable::SoundTable() {
    this->SERVER_next_id = 0;
}

void SoundTable::tickSounds() {
    for (auto it = this->map.begin(); it != this->map.end(); ) {
        auto& [id, source] = *it;
        if (!source.has_value()) {
            continue;
        }

        source->SERVER_ticks_remaining--;
        if (source->SERVER_ticks_remaining == 0) {
            it = this->map.erase(it);
        } else {
            it++;
        }
    }
}

void SoundTable::addNewSoundSource(SoundSource source) {
    this->map.insert({this->SERVER_next_id++, source});
}

void SoundTable::updateSoundSource(SoundID id, std::optional<SoundSource> source) {
    this->map[id] = source;
}

const std::unordered_map<SoundID, std::optional<SoundSource>> SoundTable::data() const {
    return this->map;
}