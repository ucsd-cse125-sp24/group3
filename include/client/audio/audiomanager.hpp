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

	void playMusic(ClientMusic music);
	void pauseMusic(ClientMusic music);
	void stopMusic(ClientMusic music);
	void playSFX(ClientSFX sfx);

	void AudioManager::doTick(glm::vec3 player_pos, const SoundTable& delta);

private:
	/**
	 * Updates the record of sounds received from the server
	 * 
	 * @param delta Changes in the sound table from the previous tick
	 */
	void updateSoundTable(const SoundTable& delta);

	std::unordered_map<ClientMusic, std::unique_ptr<sf::Music>> clientMusics;

	std::unordered_map<ClientSFX, sf::SoundBuffer> clientSFXBufs;
	std::unordered_map<ClientSFX, std::unique_ptr<sf::Sound>> clientSFXs;

	std::unordered_map<ServerSFX, sf::SoundBuffer> serverSFXBufs;
	SoundTable serverTable;
	std::unordered_map<SoundID, std::unique_ptr<sf::Sound>> serverSFXs;

	sf::SoundBuffer loadSFXBuf(ClientSFX sfx);
	sf::SoundBuffer loadSFXBuf(ServerSFX sfx);
	sf::SoundBuffer loadSFXBuf(std::string path);

	std::unique_ptr<sf::Sound> makeSound(const SoundSource& source);
	std::unique_ptr<sf::Music> makeMusic(ClientMusic music);
};