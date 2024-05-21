#include "shared/audio/soundsource.hpp"
#include "shared/audio/soundtype.hpp"

#include <unordered_map>
#include <chrono>

SoundSource::SoundSource(ServerSFX sfx, glm::vec3 pos, float volume, float min_dist, float atten, bool loop):
    sfx(sfx), pos(pos), volume(volume), min_dist(min_dist), atten(atten), loop(loop)
{
    if (!loop) {
        this->SERVER_time_remaining = SERVER_SFX_LENS.at(sfx);
    }
}

bool SoundSource::canBeHeardFrom(glm::vec3 pos) const {
    // https://www.sfml-dev.org/tutorials/1.6/audio-spatialization.php#:~:text=The%20attenuation%20is%20a%20multiplicative,very%20close%20to%20the%20listener.
    // Factor = MinDistance / (MinDistance + Attenuation * (max(Distance, MinDistance) - MinDistance))

    float dist = glm::distance(pos, this->pos);

    float factor = min_dist / (min_dist + atten + std::max(dist, min_dist) - min_dist);

    return (factor > 0.0f);
}