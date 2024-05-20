#include "client/audio/audiomanager.hpp"
#include "shared/audio/soundtype.hpp"
#include "shared/audio/soundsource.hpp"
#include "shared/utilities/root_path.hpp"
#include <iostream>

using namespace std;

AudioManager::AudioManager() {

}

AudioManager::~AudioManager() {

}

void AudioManager::init() {
	std::cout << "Initializing Audio Manager...\n";
	this->loadClientSounds();
}

void AudioManager::updateSoundTable(const SoundTable& delta) {
	for (const auto& [id, source] : delta.data()) {
		if (this->)
	}
}

void AudioManager::changeClientVolume(ClientSound type, float volume) {
	this->clientSounds[type]->setVolume(volume);
}

void AudioManager::loadClientSounds() {
	for (auto sound_type : ALL_CLIENT_SOUNDS()) {
		auto path = soundTypeToPath(sound_type);
		std::cout << "Loading " << path << "\n";

		auto sound = loadSound(path);
		if (sound == nullptr) {
			std::cerr<< "Failed to load" << std::endl;
			std::exit(1);
		}

		this->clientSounds[sound_type] = sound;
	}
}

void AudioManager::pauseClientAudio(ClientSound type) {
	this->clientSounds[type]->pause();
}

void AudioManager::playClientAudio(ClientSound type) {
	this->clientSounds[type]->play();
}

void AudioManager::loopClientAudio(ClientSound type) {
	// hacky way of setting loops, will change
	if (type == ClientSound::TitleTheme || type == ClientSound::GameTheme) {
		auto musicPtr = std::dynamic_pointer_cast<sf::Music>(this->clientSounds[type]);

		if (musicPtr) {
			musicPtr->setLoop(true);
		}
		else {
			// Handle the case where the object in the shared_ptr is not an sf::Music
			std::cout << "Error: Not an sf::Music object." << std::endl;
		}
	}
}

// void AudioManager::setAttenuation(SoundType type, float attenuation) {
// 	this->soundMap[type]->setAttenuation(attenuation);
// }

// void AudioManager::setSoundMinDistance(SoundType type, float distance) {
// 	this->soundMap[type]->setMinDistance(distance);
// }

// void AudioManager::setSoundPosition(SoundType type, glm::vec3 pos) {
// 	this->soundMap[type]->setPosition(pos.x, pos.y, pos.z);
// }

std::shared_ptr<sf::Sound> AudioManager::loadSound(std::string path) {
	auto buffer = std::make_shared<sf::SoundBuffer>();

	if (!buffer->loadFromFile(path)) {
		return nullptr;
	}

	return std::make_shared<sf::Sound>(*buffer);
}