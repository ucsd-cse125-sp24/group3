#pragma once

#include <unordered_map>
#include <boost/filesystem.hpp>
#include <SFML/Audio.hpp>
#include <memory>

#include "client/core.hpp"
#include "shared/audio/soundtype.hpp"
#include "shared/audio/soundsource.hpp"
#include "shared/audio/soundtable.hpp"

class AudioManager {
public:
	AudioManager();
	~AudioManager();

	/**
	 * load in all the sound & music files
	 */
	void init();

	/**
	 * Updates the record of sounds received from the server
	 * 
	 * @param delta Changes in the sound table from the previous tick
	 */
	void updateSoundTable(const SoundTable& delta);

	/**
	 * Play the audio of a particular SoundType
	 *
	 * @param the SoundType to play
	 */
	void playClientAudio(ClientSound type);

	/**
	 * Pause the audio of a particular SoundType
	 *
	 * @param the SoundType to play
	 */
	void pauseClientAudio(ClientSound type);

	/**
	 * Pause the audio of a particular SoundType
	 *
	 * @param the SoundType to play
	 */
	void loopClientAudio(ClientSound type);

	/**
	 * Change the volume of a particular SoundType
	 * The volume is a value between 0 (mute) and 100 (full volume). 
	 * The default value for the volume is 100.
	 * 
	 * @param the SoundType to change
	 * #param the volume to set it to
	 */
	void changeClientVolume(ClientSound type, float volume);

	// void setAttenuation(SoundType type, float attenuation);

	// void setSoundMinDistance(SoundType type, float distance);

	// void setSoundPosition(SoundType type, glm::vec3 pos);

private:
	std::unordered_map<ClientSound, std::shared_ptr<sf::SoundSource>> clientSounds;

	SoundTable serverTable;
	std::unordered_map<SoundID, std::unique_ptr<sf::SoundSource>> serverSounds;

	void loadClientSounds();

	std::shared_ptr<sf::Sound> loadSound(std::string path);
};