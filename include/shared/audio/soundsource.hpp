#pragma once

#include <glm/glm.hpp>
#include "shared/audio/soundtype.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "shared/utilities/typedefs.hpp"

struct SoundSource {
    SoundSource(ServerSound sound, glm::vec3 pos, float volume, float min_dist, float atten);

    ServerSound sound;
    glm::vec3 pos;
    float volume;
    float min_dist;
    float attenuation;

    size_t SERVER_ticks_remaining; // not serialized b/c not used by client

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & sound & pos & volume & min_dist & attenuation;
    }
};