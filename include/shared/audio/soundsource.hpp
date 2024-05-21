#pragma once

#include <chrono>
#include <glm/glm.hpp>
#include "shared/audio/soundtype.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "shared/utilities/typedefs.hpp"

struct SoundSource {
    SoundSource() = default;
    SoundSource(ServerSFX sfx, glm::vec3 pos, float volume, float min_dist, float atten);

    ServerSFX sfx;
    glm::vec3 pos;
    float volume;
    float min_dist;
    float atten;

    std::chrono::milliseconds SERVER_time_remaining; // not serialized b/c not used by client

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & sfx & pos & volume & min_dist & atten;
    }
};