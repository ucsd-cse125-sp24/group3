#include "shared/audio/soundtype.hpp"
#include "shared/utilities/root_path.hpp"

#include <string>

static auto audio_dir = getRepoRoot() / "assets" / "sounds";

std::string soundTypeToPath(ServerSound sound) {
    switch (sound) {
        case ServerSound::ArrowShoot:
            return (audio_dir / "arrow.wav").string();
    }
}

std::string soundTypeToPath(ClientSound sound) {
    switch (sound) {
        case ClientSound::TitleTheme:
            return (audio_dir / "piano.wav").string();
        case ClientSound::GameTheme:
            return (audio_dir / "mono-retrowave.mp3").string();
    }
}