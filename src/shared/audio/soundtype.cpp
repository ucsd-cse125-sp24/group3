#include "shared/audio/soundtype.hpp"
#include "shared/utilities/root_path.hpp"

#include <iostream>
#include <string>

static auto audio_dir = getRepoRoot() / "assets" / "sounds";

std::string getAudioPath(ClientSFX sound) {
    switch (sound) {
        case ClientSFX::TEMP:
            return (audio_dir / "vine-boom-mono.mp3").string();
        default:
            std::cerr << "FATAL: no known path for ClientSFX " << static_cast<int>(sound) << std::endl;
            std::exit(1);
    }
}

std::string getAudioPath(ServerSFX sfx) {
    static auto dir = audio_dir / "server_sfx";
    switch (sfx) {
        case ServerSFX::ArrowShoot:
            return (dir / "arrow_shoot_mono.wav").string();
        case ServerSFX::ArrowImpact:
            return (dir / "arrow_impact_mono.wav").string(); 
        case ServerSFX::FireballShoot:
            return (dir / "fireball_shoot_mono.wav").string(); 
        case ServerSFX::FireballImpact:
            return (dir / "fireball_impact_mono.wav").string(); 
        case ServerSFX::SlimeJump:
            return (dir / "slime_jump_mono.wav").string();
        case ServerSFX::SlimeLand:
            return (dir / "slime_land_mono.wav").string();
        case ServerSFX::PlayerJump:
            return (dir / "player_jump_mono.wav").string();
        case ServerSFX::PlayerLand:
            return (dir / "player_land_mono.wav").string();
        case ServerSFX::PlayerWalk1:
            return (dir / "player_walk_1_mono.wav").string();
        case ServerSFX::PlayerWalk2:
            return (dir / "player_walk_2_mono.wav").string();
        case ServerSFX::PlayerWalk3:
            return (dir / "player_walk_3_mono.wav").string();
        case ServerSFX::PlayerWalk4:
            return (dir / "player_walk_4_mono.wav").string();
        case ServerSFX::PlayerWalk5:
            return (dir / "player_walk_5_mono.wav").string();
        case ServerSFX::CeilingSpikeImpact:
            return (dir / "ceiling_spike_impact_short_mono.wav").string();
        case ServerSFX::CeilingSpikeTrigger:
            return (dir / "ceiling_spike_trigger_mono.wav").string();
        
        default:
            std::cerr << "FATAL: no known path for ServerSFX " << static_cast<int>(sfx) << std::endl;
            std::exit(1);
    }
}

std::string getAudioPath(ClientMusic music) {
    switch (music) {
        case ClientMusic::TitleTheme:
            return (audio_dir / "piano.wav").string();
        case ClientMusic::GameTheme:
            return (audio_dir / "mono-retrowave.mp3").string();
        default:
            std::cerr << "FATAL: no known path for ClientMusic " << static_cast<int>(music) << std::endl;
            std::exit(1);
    }
}