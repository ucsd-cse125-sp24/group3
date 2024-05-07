#include "client/audiomanager.hpp"
#include <iostream>

using namespace std;

AudioManager::AudioManager() {

}

AudioManager::~AudioManager() {

}

void AudioManager::changeVolume(SoundType type, float volume) {
	this->soundMap[type]->setVolume(volume);
}

void AudioManager::loadAudioFiles(std::vector<std::pair<boost::filesystem::path, SoundType>> audioPaths) {
	for (std::pair<boost::filesystem::path, SoundType> audioPath : audioPaths) {
		std::unique_ptr<sf::SoundBuffer> buffer = std::make_unique<sf::SoundBuffer>();

		boost::filesystem::path soundFilePath = audioPath.first;

		std::cout << "Audio Path: " << soundFilePath.string() << std::endl;

		if (!buffer->loadFromFile(soundFilePath.string())) {
			std::cout << "failed to load audio" << std::endl;
			return;
		}

		std::shared_ptr<sf::Sound> sound = std::make_shared<sf::Sound>(*buffer);

		SoundType type = audioPath.second;

		this->soundMap[type] = sound;

		std::cout << "added sound" << std::endl;
	}
}

void AudioManager::loadMusicFiles(std::vector<std::pair<boost::filesystem::path, SoundType>> musicPaths) {
	for (std::pair<boost::filesystem::path, SoundType> musicPath : musicPaths) {
		std::unique_ptr<sf::Music> music = std::make_unique<sf::Music>();

		boost::filesystem::path soundFilePath = musicPath.first;

		std::cout << "Music Path: " << soundFilePath.string() << std::endl;

		if (!music->openFromFile(soundFilePath.string())) {
			std::cout << "failed to load music" << std::endl;
			return;
		}

		SoundType type = musicPath.second;

		this->soundMap[type] = std::move(music);

		std::cout << "added music" << std::endl;
	}
}

void AudioManager::pauseAudio(SoundType type) {
	this->soundMap[type]->pause();
}

void AudioManager::playAudio(SoundType type) {
	std::cout << "playing audio" << std::endl;
	this->soundMap[type]->play();
}

void AudioManager::loop(SoundType type) {
	// hacky way of setting loops, will change
	if (type == SoundType::Background) {
		auto musicPtr = std::dynamic_pointer_cast<sf::Music>(this->soundMap[type]);

		if (musicPtr) {
			musicPtr->setLoop(true);
		}
		else {
			// Handle the case where the object in the shared_ptr is not an sf::Music
			std::cout << "Error: Not an sf::Music object." << std::endl;
		}
	}
}