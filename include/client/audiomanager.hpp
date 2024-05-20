#pragma once

#include <unordered_map>
#include <boost/filesystem.hpp>
#include <SFML/Audio.hpp>
#include <memory>

#include "client/core.hpp"

enum class SoundType {
    Collision,
    Background
};

class AudioManager {
public:
	AudioManager();
	~AudioManager();

	/**
	 * load in all the sound & music files
	 */
	void init();

	/**
	 * Play the audio of a particular SoundType
	 *
	 * @param the SoundType to play
	 */
	void playAudio(SoundType type);

	/**
	 * Pause the audio of a particular SoundType
	 *
	 * @param the SoundType to play
	 */
	void pauseAudio(SoundType type);

	/**
	 * Pause the audio of a particular SoundType
	 *
	 * @param the SoundType to play
	 */
	void loop(SoundType type);

	/**
	 * Change the volume of a particular SoundType
	 * The volume is a value between 0 (mute) and 100 (full volume). 
	 * The default value for the volume is 100.
	 * 
	 * @param the SoundType to change
	 * #param the volume to set it to
	 */
	void changeVolume(SoundType type, float volume);

	void setAttenuation(SoundType type, float attenuation);

	void setSoundMinDistance(SoundType type, float distance);

	void setSoundPosition(SoundType type, glm::vec3 pos);

private:
	std::unordered_map<SoundType, std::shared_ptr<sf::SoundSource>> soundMap;

	void loadAudioFiles(std::vector< std::pair<boost::filesystem::path, SoundType>> audioPaths);

	void loadMusicFiles(std::vector< std::pair<boost::filesystem::path, SoundType>> musicPaths);
};