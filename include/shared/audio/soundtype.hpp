#pragma once

#include <string>
#include <chrono>
#include <unordered_map>

using namespace std::chrono_literals;

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
    ArrowImpact,
    FireballShoot,
    FireballImpact,
    SlimeJump,
    SlimeLand,
    PlayerJump,
    PlayerLand,
    PlayerWalk1,
    PlayerWalk2,
    PlayerWalk3,
    PlayerWalk4,
    PlayerWalk5,
    CeilingSpikeTrigger,
    CeilingSpikeImpact,
    TorchLoop,
    Thunder,
    // make sure to add to server sfx len map!
    // make sure to add to macro below!
};

const std::unordered_map<ServerSFX, std::chrono::milliseconds> SERVER_SFX_LENS = {
    {ServerSFX::ArrowShoot,          620ms},
    {ServerSFX::ArrowImpact,        1030ms},
    {ServerSFX::FireballShoot,      1220ms},
    {ServerSFX::FireballImpact,      370ms},
    {ServerSFX::SlimeJump,           610ms},
    {ServerSFX::SlimeLand,           760ms},
    {ServerSFX::PlayerJump,          500ms},
    {ServerSFX::PlayerLand,          500ms},
    {ServerSFX::PlayerWalk1,         500ms},
    {ServerSFX::PlayerWalk2,         500ms},
    {ServerSFX::PlayerWalk3,         500ms},
    {ServerSFX::PlayerWalk4,         500ms},
    {ServerSFX::PlayerWalk5,         500ms},
    {ServerSFX::CeilingSpikeTrigger, 380ms},
    {ServerSFX::CeilingSpikeImpact, 1180ms},
    {ServerSFX::Thunder,            5000ms},
    {ServerSFX::TorchLoop,          9999ms} // wont actually be used because it loops
    // dont forget macro below!
};

#define GET_SERVER_SFXS() { \
    ServerSFX::ArrowShoot, ServerSFX::FireballShoot, ServerSFX::ArrowImpact, ServerSFX::FireballImpact, \
    ServerSFX::SlimeJump, ServerSFX::SlimeLand, ServerSFX::PlayerJump, ServerSFX::PlayerLand, \
    ServerSFX::PlayerWalk1, ServerSFX::PlayerWalk2, ServerSFX::PlayerWalk3, ServerSFX::PlayerWalk4, ServerSFX::PlayerWalk5, \
    ServerSFX::CeilingSpikeTrigger, ServerSFX::CeilingSpikeImpact, ServerSFX::TorchLoop, ServerSFX::Thunder,\
}

// const std::unordered_map<ServerSound, size_t> serverSoundTickLengths = {
//     {ServerSound::ArrowShoot, 100},
// };

std::string getAudioPath(ClientMusic music);
std::string getAudioPath(ClientSFX sfx);
std::string getAudioPath(ServerSFX sfx);