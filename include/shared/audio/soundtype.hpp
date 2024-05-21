#pragma once

#include <string>

// Sounds that the client can decide to play on its own
// (e.g. BGM, clicking on UI elements...)
enum class ClientMusic {
    TitleTheme,
    GameTheme,
    // make sure to add to macro below!
};

#define GET_CLIENT_MUSICS() { \
    ClientMusic::TitleTheme, ClientMusic::GameTheme \
}

enum class ClientSFX {
    // TODO: decide what these are
    TEMP,
    // make sure to add to macro below!
};

#define GET_CLIENT_SFXS() { \
    ClientSFX::TEMP, \ 
}

// Sounds that correspond to something in the game world
enum class ServerSFX {
    ArrowShoot,
    FireballShoot,
    // make sure to add to macro below!
};

#define GET_SERVER_SFXS() { \
    ServerSFX::ArrowShoot, ServerSFX::FireballShoot, \
}

// const std::unordered_map<ServerSound, size_t> serverSoundTickLengths = {
//     {ServerSound::ArrowShoot, 100},
// };

std::string getAudioPath(ClientMusic music);
std::string getAudioPath(ClientSFX sfx);
std::string getAudioPath(ServerSFX sfx);