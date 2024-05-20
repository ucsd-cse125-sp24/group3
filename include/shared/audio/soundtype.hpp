#pragma once

#include <string>

// Sounds that the client can decide to play on its own
// (e.g. BGM, clicking on UI elements...)
enum class ClientSound {
    TitleTheme,
    GameTheme,
};

#define ALL_CLIENT_SOUNDS() { \
    ClientSound::TitleTheme, ClientSound::GameTheme \
}

// Sounds that correspond to something in the game world
enum class ServerSound {
    ArrowShoot
};

const std::unordered_map<ServerSound, size_t> serverSoundTickLengths = {
    {ServerSound::ArrowShoot, 100},
};

std::string soundTypeToPath(ClientSound sound);
std::string soundTypeToPath(ServerSound sound);