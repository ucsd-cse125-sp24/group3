#pragma once

#include <deque>
#include <boost/optional.hpp>
#include "shared/audio/soundsource.hpp"
#include "shared/utilities/serialize_macro.hpp"

using SoundID = size_t;

class SoundTable {
public:
    SoundTable();

    void addNewSoundSource(SoundSource source);
    void updateSoundSource(SoundID id, boost::optional<SoundSource> source);
    void tickSounds();

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & map;
    }

    const std::unordered_map<SoundID, boost::optional<SoundSource>> data() const;

private:
    std::unordered_map<SoundID, boost::optional<SoundSource>> map;

    std::deque<SoundID> id_free_list;
    SoundID SERVER_next_id;
};
