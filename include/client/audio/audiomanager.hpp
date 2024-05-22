#pragma once

#include <unordered_map>
#include <boost/filesystem.hpp>
#include <SFML/Audio.hpp>
#include <memory>
#include <string>

#include "client/core.hpp"
#include "shared/audio/soundtype.hpp"
#include "shared/audio/soundsource.hpp"
#include "shared/audio/soundcommand.hpp"
#include "shared/game/event.hpp"

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

	void doTick(glm::vec3 player_pos, 
		const LoadSoundCommandsEvent& delta, 
		std::array<boost::optional<SharedObject>, MAX_POINT_LIGHTS> light_sources);

private:
	std::unordered_map<ClientMusic, std::unique_ptr<sf::Music>> clientMusics;

	std::unordered_map<ClientSFX, std::unique_ptr<sf::SoundBuffer>> clientSFXBufs;
	std::unordered_map<ClientSFX, std::unique_ptr<sf::Sound>> clientSFXs;

	std::unordered_map<ServerSFX, std::unique_ptr<sf::SoundBuffer>> serverSFXBufs;
	std::unordered_map<SoundID, std::unique_ptr<sf::Sound>> serverSFXs;

	std::array<std::unique_ptr<sf::Sound>, MAX_POINT_LIGHTS> serverLightSFXs;

	std::unique_ptr<sf::SoundBuffer> loadSFXBuf(ClientSFX sfx);
	std::unique_ptr<sf::SoundBuffer> loadSFXBuf(ServerSFX sfx);
	std::unique_ptr<sf::SoundBuffer> loadSFXBuf(std::string path);

	void setSoundParams(sf::Sound& sound, const SoundSource& source);

	std::unique_ptr<sf::Sound> makeSound(const SoundSource& source);
	std::unique_ptr<sf::Music> makeMusic(ClientMusic music);
};