#pragma once

#include "shared/audio/soundsource.hpp"
#include "shared/utilities/serialize_macro.hpp"

using SoundID = unsigned long long;

enum class SoundAction {
    PLAY,
    DO_DELETE // can't just be DELETE because that is already being set by some other macro on windows
};

struct SoundCommand {
    SoundCommand() = default;
    SoundCommand(SoundID id, SoundAction action, const SoundSource& source):
        id(id), action(action), source(source) {}

    SoundID id;
    SoundAction action;
    SoundSource source;

    DEF_SERIALIZE(Archive& ar, const unsigned int version) {
        ar & id & action & source;
    }
};
