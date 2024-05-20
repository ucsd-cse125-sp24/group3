#pragma once

#include "shared/audio/soundsource.hpp"
#include "shared/utilities/serialize_macro.hpp"

using SoundID = size_t;

class SoundTable {
public:
    SoundTable();

    void addNewSoundSource(SoundSource source);
    void updateSoundSource(SoundID id, std::optional<SoundSource> source);
    void tickSounds();

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & map;
    }

    const std::unordered_map<SoundID, std::optional<SoundSource>> data() const;

private:
    std::unordered_map<SoundID, std::optional<SoundSource>> map;

    SoundID SERVER_next_id;
};
