#include "client/audiomanager.hpp"

AudioManager::AudioManager() {

}

AudioManager::~AudioManager() {

}

void AudioManager::changeVolume(SoundType type, float volume) {
	this->soundMap[type].setVolume(volume);
}

void AudioManager::loadAudioFiles(std::vector<std::pair<boost::filesystem::path, SoundType>> audioPaths) {
	for (std::pair<boost::filesystem::path, SoundType> audioPath : audioPaths) {
		sf::SoundBuffer buffer;

		boost::filesystem::path soundFilePath = audioPath.first;

		if (!buffer.loadFromFile(soundFilePath.string()))
			return;

		sf::Sound sound;
		sound.setBuffer(buffer);

		SoundType type = audioPath.second;

		this->soundMap[type] = sound;
	}
}

void AudioManager::pauseAudio(SoundType type) {
	this->soundMap[type].pause();
}

void AudioManager::playAudio(SoundType type) {
	this->soundMap[type].play();
}