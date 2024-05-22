#include "client/audio/audiomanager.hpp"
#include "shared/audio/soundtype.hpp"
#include "shared/audio/soundsource.hpp"
#include "shared/audio/soundcommand.hpp"
#include "shared/utilities/root_path.hpp"
#include "shared/game/event.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <string>

using namespace std;

AudioManager::AudioManager() {

}

AudioManager::~AudioManager() {

}

void AudioManager::init() {
    std::cout << "Initializing Audio Manager...\n";

    for (auto type : GET_CLIENT_MUSICS()) {
        this->clientMusics.insert({type, this->makeMusic(type)});
        // these are just streamed from the files at play time, so we are done for initing
    }	
    for (auto type : GET_CLIENT_SFXS()) {
        auto buf = this->loadSFXBuf(type);
        auto sfx = std::make_unique<sf::Sound>();
        sfx->setBuffer(*buf);
        sfx->setRelativeToListener(true); // make it not dependent on the position of player
        this->clientSFXBufs.insert({type, std::move(buf)});	

        // can make sf::Sound objects right away, since there will only ever be 1 of them playing at a time
        this->clientSFXs.insert({type, std::move(sfx)});
    }
    for (auto type : GET_SERVER_SFXS()) {
        this->serverSFXBufs.insert({type, this->loadSFXBuf(type)});
        // dont make sound objects right now, because will need to dynamically make them as the server
        // sends sound tables
    }
}

void AudioManager::playMusic(ClientMusic music) {
    this->clientMusics.at(music)->play();	
}

void AudioManager::pauseMusic(ClientMusic music) {
    this->clientMusics.at(music)->pause();
}

void AudioManager::stopMusic(ClientMusic music) {
    this->clientMusics.at(music)->stop();	
}

void AudioManager::playSFX(ClientSFX sfx) {
    this->clientSFXs.at(sfx)->play();
}

void AudioManager::doTick(glm::vec3 player_pos, const LoadSoundCommandsEvent& event) {
    sf::Listener::setPosition(player_pos.x, player_pos.y, player_pos.z);

    for (const auto& command : event.commands) {
        switch (command.action) {
            case SoundAction::PLAY: {
                if (this->serverSFXs.contains(command.id)) {
                    continue; // ignore the command because this id is already playing, this is expected to happen
                }	
                auto sound = this->makeSound(command.source);
                sound->play();
                this->serverSFXs.insert({command.id, std::move(sound)});
                break;
            }
            case SoundAction::DELETE:
                if (!this->serverSFXs.contains(command.id)) {
                    continue; // ignore the command because we aren't even playing this sound
                }

                this->serverSFXs.at(command.id)->stop();
                this->serverSFXs.erase(command.id);
                break;
        }
    }	
}

std::unique_ptr<sf::SoundBuffer> AudioManager::loadSFXBuf(ClientSFX sfx) {
    return this->loadSFXBuf(getAudioPath(sfx));
}

std::unique_ptr<sf::SoundBuffer> AudioManager::loadSFXBuf(ServerSFX sfx) {
    return this->loadSFXBuf(getAudioPath(sfx));
}

std::unique_ptr<sf::SoundBuffer> AudioManager::loadSFXBuf(std::string path) {
    auto buffer = std::make_unique<sf::SoundBuffer>();

    std::cout << "Loading " << path << "\n";
    if (!buffer->loadFromFile(path)) {
        std::cout << "Failed to load " << path << std::endl;
        std::exit(1);
    }

    return std::move(buffer);
}

void AudioManager::setSoundParams(sf::Sound& sound, const SoundSource& source) {
    sound.setBuffer(*this->serverSFXBufs.at(source.sfx));
    sound.setAttenuation(source.atten);
    sound.setMinDistance(source.min_dist);
    sound.setPosition(source.pos.x, source.pos.y, source.pos.z);
    sound.setVolume(source.volume);
    sound.setLoop(source.loop);
}

std::unique_ptr<sf::Sound> AudioManager::makeSound(const SoundSource& source) {
    auto sound = std::make_unique<sf::Sound>();
    this->setSoundParams(*sound, source);
    return std::move(sound);
}

std::unique_ptr<sf::Music> AudioManager::makeMusic(ClientMusic music_type) {
    auto music = std::make_unique<sf::Music>();
    auto path = getAudioPath(music_type);
    std::cout << "Opening " << path << "\n";
    if (!music->openFromFile(path)) {
        std::cout << "Failed to load " << path << std::endl;
        std::exit(1);
    }
    music->setLoop(true);
    music->setRelativeToListener(true); // make it not dependent on position of player
    return std::move(music);
}