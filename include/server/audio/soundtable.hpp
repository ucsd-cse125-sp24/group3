#pragma once

#include <boost/optional.hpp>
#include <unordered_map>
#include <vector>

#include "shared/audio/soundsource.hpp"
#include "shared/utilities/serialize_macro.hpp"
#include "shared/audio/soundcommand.hpp"
#include "shared/utilities/typedefs.hpp"
#include "server/game/object.hpp"
#include "shared/utilities/smartvector.hpp"

class Player;

class SoundTable {
public:
    SoundTable();

    void addNewSoundSource(const SoundSource& source);
    std::unordered_map<EntityID, std::vector<SoundCommand>> getCommandsPerPlayer(SmartVector<Player*>& players);
    void tickSounds();

    const std::unordered_map<SoundID, SoundSource>& data() const;

private:
    std::unordered_map<SoundID, SoundSource> map;
    std::vector<SoundCommand> current_commands;

    SoundID next_id;
};
