#include "shared/audio/soundsource.hpp"

SoundSource::SoundSource(ServerSound sound, glm::vec3 pos, float volume, float min_dist, float atten):
    sound(sound), pos(pos), volume(volume), min_dist(min_dist), attenuation(atten) 
{

}