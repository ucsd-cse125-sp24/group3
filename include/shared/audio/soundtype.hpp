#pragma once

#include <string>
#include <chrono>
#include <unordered_map>

using namespace std::chrono_literals;

// Sounds that the client can decide to play on its own
// (e.g. BGM, clicking on UI elements...)
enum class ClientMusic {
    MenuTheme,
    MazeExplorationPlayersTheme,
    MazeExplorationDMTheme,
    RelayRacePlayersTheme,
    RelayRaceDMTheme
    // make sure to add to macro below!
};

#define GET_CLIENT_MUSICS() { \
    ClientMusic::MenuTheme, ClientMusic::MazeExplorationPlayersTheme, \
    ClientMusic::MazeExplorationDMTheme, ClientMusic::RelayRacePlayersTheme, \
    ClientMusic::RelayRaceDMTheme \
}

enum class ClientSFX {
    // TODO: decide what these are
    VictoryThemePlayers,
    VictoryThemeDM,
    TEMP,
    // make sure to add to macro below!
};

#define GET_CLIENT_SFXS() { \
    ClientSFX::VictoryThemePlayers, ClientSFX::VictoryThemeDM, \
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
    Dagger,
    Sword,
    Hammer,
    Minotaur,
    Python,
    Teleport,
    Potion,
    Spell,
    ItemPickUp,
    ItemDrop,
    MirrorShatter,
    TEMP,
    PlayersStartTheme,
    ZeusStartTheme,
    ElectricHum,
    IntroGateOpen,
    Wind,
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
    {ServerSFX::Dagger,              500ms},
    {ServerSFX::Sword,               500ms},
    {ServerSFX::Hammer,             1000ms},
    {ServerSFX::Minotaur,           1000ms},
    {ServerSFX::Python,             1000ms},
    {ServerSFX::CeilingSpikeTrigger, 380ms},
    {ServerSFX::CeilingSpikeImpact, 1180ms},
    {ServerSFX::Thunder,            2500ms},

    // used not for in game, but for the intro cutscene, so this is the duration of sound in intro cutscene
    {ServerSFX::TorchLoop,         30000ms}, 
    {ServerSFX::Wind,              14000ms}, 

    {ServerSFX::PlayersStartTheme,  8000ms},
    {ServerSFX::ElectricHum,        1500ms},
    {ServerSFX::IntroGateOpen,      9000ms},
    {ServerSFX::ZeusStartTheme,    12000ms},
    {ServerSFX::Teleport,           500ms},
    {ServerSFX::Potion,             500ms},
    {ServerSFX::Spell,              500ms},
    {ServerSFX::ItemPickUp,         500ms},
    {ServerSFX::ItemDrop,           500ms},
    {ServerSFX::MirrorShatter,      2000ms},
    // dont forget macro below!
};

#define GET_SERVER_SFXS() { \
    ServerSFX::ArrowShoot, ServerSFX::FireballShoot, ServerSFX::ArrowImpact, ServerSFX::FireballImpact, \
    ServerSFX::SlimeJump, ServerSFX::SlimeLand, ServerSFX::PlayerJump, ServerSFX::PlayerLand, \
    ServerSFX::PlayerWalk1, ServerSFX::PlayerWalk2, ServerSFX::PlayerWalk3, ServerSFX::PlayerWalk4, ServerSFX::PlayerWalk5, \
    ServerSFX::CeilingSpikeTrigger, ServerSFX::CeilingSpikeImpact, ServerSFX::TorchLoop, ServerSFX::Thunder,\
    ServerSFX::Dagger, ServerSFX::Sword, ServerSFX::Hammer, ServerSFX::Minotaur, ServerSFX::Python, \
    ServerSFX::PlayersStartTheme, ServerSFX::ElectricHum, ServerSFX::IntroGateOpen, ServerSFX::ZeusStartTheme, \
    ServerSFX::Wind, \
    ServerSFX::Teleport, ServerSFX::Potion, ServerSFX::Spell, ServerSFX::ItemPickUp, ServerSFX::ItemDrop, \
    ServerSFX::MirrorShatter \
}

// const std::unordered_map<ServerSound, size_t> serverSoundTickLengths = {
//     {ServerSound::ArrowShoot, 100},
// };

std::string getAudioPath(ClientMusic music);
std::string getAudioPath(ClientSFX sfx);
std::string getAudioPath(ServerSFX sfx);