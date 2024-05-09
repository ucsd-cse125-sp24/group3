#include "server/game/spiketrap.hpp"
#include "server/game/servergamestate.hpp"

SpikeTrap::SpikeTrap():
    Trap(ObjectType::SpikeTrap) 
{
    this->physics.movable = false;
}

bool SpikeTrap::shouldTrigger(const Object& obj) {
    if (this->info.triggered) {
        return false;
    }
    
    if (obj.type == ObjectType::Player) {
        bool is_underneath = (
            obj.physics.shared.position.x >= this->physics.shared.corner.x &&
            obj.physics.shared.position.x <= this->physics.shared.corner.x + this->physics.shared.dimensions.x &&
            obj.physics.shared.position.z >= this->physics.shared.corner.z &&
            obj.physics.shared.position.z <= this->physics.shared.corner.z + this->physics.shared.dimensions.z
        );

        if (is_underneath) {
            return true;
        }
    }

    return false;
}

void SpikeTrap::trigger() {
    Trap::trigger();

    this->physics.movable = true;
    this->physics.velocity.y = -GRAVITY;
}