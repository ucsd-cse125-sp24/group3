#include "shared/audio/soundtype.hpp"
#include "shared/utilities/root_path.hpp"

#include <string>

static auto audio_dir = getRepoRoot() / "assets" / "sounds";

std::string getAudioPath(ClientSFX sound) {
    switch (sound) {
        case ClientSFX::TEMP:
        default:
            return (audio_dir / "arrow.wav").string();
    }
}

std::string getAudioPath(ServerSFX sfx) {
    switch (sfx) {
        case ServerSFX::ArrowShoot:
            return (audio_dir / "arrow.wav").string();
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