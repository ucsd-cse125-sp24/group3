#include "shared/audio/soundtype.hpp"
#include "shared/utilities/root_path.hpp"

#include <string>

static auto audio_dir = getRepoRoot() / "assets" / "sounds";

std::string getAudioPath(ClientSFX sound) {
    switch (sound) {
        case ClientSFX::TEMP:
        default:
            return (audio_dir / "vine-boom-mono.mp3").string();
    }
}

std::string getAudioPath(ServerSFX sfx) {
    static auto dir = audio_dir / "server_sfx";
    switch (sfx) {
        case ServerSFX::ArrowShoot:
            return (dir / "arrow_shoot.wav").string();
        case ServerSFX::FireballShoot:
            return (dir / "fireball_shoot.wav").string(); 
    }
}

std::string getAudioPath(ClientMusic music) {
    switch (music) {
        case ClientMusic::TitleTheme:
            return (audio_dir / "piano.wav").string();
        case ClientMusic::GameTheme:
            return (audio_dir / "mono-retrowave.mp3").string();
    }
}