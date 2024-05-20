#include "shared/audio/soundsource.hpp"

SoundSource::SoundSource(ServerSFX sfx, glm::vec3 pos, float volume, float min_dist, float atten):
    sfx(sfx), pos(pos), volume(volume), min_dist(min_dist), atten(atten) 
{

}