#include "shared/audio/soundtype.hpp"
#include "shared/utilities/root_path.hpp"

#include <iostream>
#include <string>

static auto audio_dir = getRepoRoot() / "assets" / "sounds";

std::string getAudioPath(ClientSFX sound) {
    static auto dir = audio_dir / "client_sfx";
    switch (sound) {
        case ClientSFX::TEMP:
            return (dir / "vine-boom-mono.mp3").string();
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
        case ServerSFX::Dagger:
            return (dir / "dagger.wav").string();
        case ServerSFX::Sword:
            return (dir / "sword.wav").string();
        case ServerSFX::Hammer:
            return (dir / "hammer.wav").string();
        case ServerSFX::Minotaur:
            return (dir / "minotaur.wav").string();
        case ServerSFX::Python:
            return (dir / "python.wav").string();
        case ServerSFX::CeilingSpikeImpact:
            return (dir / "ceiling_spike_impact_short_mono.wav").string();
        case ServerSFX::CeilingSpikeTrigger:
            return (dir / "ceiling_spike_trigger_mono.wav").string();
        case ServerSFX::Thunder:
            return (dir / "thunder.wav").string();
        case ServerSFX::TorchLoop:
            return (dir / "torch_loop_mono.wav").string();
        case ServerSFX::Teleport:
            return (dir / "teleport.wav").string();
        case ServerSFX::Potion:
            return (dir / "potion.wav").string();
        case ServerSFX::Spell:
            return (dir / "spell.wav").string();
        case ServerSFX::ItemPickUp:
            return (dir / "itempickup.wav").string();
        case ServerSFX::ItemDrop:
            return (dir / "itemdrop.wav").string();
        case ServerSFX::MirrorShatter:
            return (dir / "mirror_shatter.mp3").string();
        
        default:
            std::cerr << "FATAL: no known path for ServerSFX " << static_cast<int>(sfx) << std::endl;
            std::exit(1);
    }
}

std::string getAudioPath(ClientMusic music) {
    static auto dir = audio_dir / "client_music";

    switch (music) {
        case ClientMusic::TitleTheme:
            return (dir / "piano.wav").string();
        case ClientMusic::GameTheme:
            return (dir / "dm_maze.flac").string();
        default:
            std::cerr << "FATAL: no known path for ClientMusic " << static_cast<int>(music) << std::endl;
            std::exit(1);
    }
}