#include "shared/audio/utilities.hpp"
#include <iostream>

ServerSFX getNextPlayerFootstep(EntityID player_eid) {
    static std::unordered_map<EntityID, unsigned int> next_footstep;
    if (!next_footstep.contains(player_eid)) {
        next_footstep.insert({player_eid, 0});
    }

    unsigned int footstep_count = next_footstep.at(player_eid)++;

    switch (footstep_count % 5) {
        case 0: return ServerSFX::PlayerWalk1;
        case 1: return ServerSFX::PlayerWalk2;
        case 2: return ServerSFX::PlayerWalk3;
        case 3: return ServerSFX::PlayerWalk4;
        case 4: return ServerSFX::PlayerWalk5;
    }

    std::cerr << "WARNING: out of bounds player footstep. WTF?\n";
    return ServerSFX::PlayerWalk1;
}