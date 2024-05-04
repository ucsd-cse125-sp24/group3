#pragma once

#include <SFML/Audio.hpp>
#include <unordered_map>
#include <boost/filesystem.hpp>

enum class SoundType {
    Collision,
    Background
};

class AudioManager {
public:
	AudioManager();
	~AudioManager();

	void loadAudioFiles(std::vector< std::pair<boost::filesystem::path, SoundType>> audioPaths);

	void playAudio(SoundType type);

	void pauseAudio(SoundType type);

	// The volume is a value between 0 (mute) and 100 (full volume). The default value for the volume is 100.
	void changeVolume(SoundType type, float volume);

private:
	std::unordered_map<SoundType, sf::Sound> soundMap;
};