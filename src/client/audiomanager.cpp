#include "client/audiomanager.hpp"
#include <iostream>

using namespace std;

AudioManager::AudioManager() {

}

AudioManager::~AudioManager() {
	for (auto sound = soundMap.begin(); sound != soundMap.end();) {
		sound = soundMap.erase(sound); // removes the element and destructs the sound
	}
}

void AudioManager::changeVolume(SoundType type, float volume) {
	this->soundMap[type]->setVolume(volume);
}

void AudioManager::loadAudioFiles(std::vector<std::pair<boost::filesystem::path, SoundType>> audioPaths) {
	for (std::pair<boost::filesystem::path, SoundType> audioPath : audioPaths) {
		sf::SoundBuffer* buffer = new sf::SoundBuffer();

		boost::filesystem::path soundFilePath = audioPath.first;

		std::cout << "Audio Path: " << soundFilePath.string() << std::endl;

		if (!buffer->loadFromFile(soundFilePath.string())) {
			std::cout << "failed to load audio" << std::endl;
			return;
		}

		sf::Sound* sound = new sf::Sound(*buffer);

		SoundType type = audioPath.second;

		this->soundMap[type] = sound;

		std::cout << "added sound" << std::endl;
	}
}

void AudioManager::loadMusicFiles(std::vector<std::pair<boost::filesystem::path, SoundType>> musicPaths) {
	for (std::pair<boost::filesystem::path, SoundType> musicPath : musicPaths) {
		sf::Music* music = new sf::Music();

		boost::filesystem::path soundFilePath = musicPath.first;

		std::cout << "Music Path: " << soundFilePath.string() << std::endl;

		if (!music->openFromFile(soundFilePath.string())) {
			std::cout << "failed to load music" << std::endl;
			return;
		}

		SoundType type = musicPath.second;

		this->soundMap[type] = music;

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
	if(type == SoundType::Background)
		((sf::Music*)(this->soundMap[type]))->setLoop(true);
}