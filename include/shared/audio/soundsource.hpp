#pragma once

#include <chrono>
#include <optional>
#include <glm/glm.hpp>
#include "shared/audio/soundtype.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "shared/utilities/typedefs.hpp"

struct SoundSource {
    SoundSource() = default;
    SoundSource(ServerSFX sfx, glm::vec3 pos, float volume, float min_dist, float atten, bool loop = false);

    ServerSFX sfx;
    glm::vec3 pos;
    float volume;
    float min_dist;
    float atten;
    bool loop;

    std::optional<std::chrono::milliseconds> SERVER_time_remaining; // not serialized b/c not used by client

    bool canBeHeardFrom(glm::vec3 pos) const;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & sfx & pos & volume & min_dist & atten & loop;
    }
};