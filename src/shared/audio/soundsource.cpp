#include "shared/audio/soundsource.hpp"
#include "shared/audio/soundtype.hpp"

#include <unordered_map>
#include <chrono>

SoundSource::SoundSource(ServerSFX sfx, glm::vec3 pos, float volume, float min_dist, float atten):
    sfx(sfx), pos(pos), volume(volume), min_dist(min_dist), atten(atten) 
{
    this->SERVER_time_remaining = SERVER_SFX_LENS.at(sfx);
}