#include "server/game/python.hpp"
#include "server/game/enemy.hpp"
#include "server/game/servergamestate.hpp"
#include "server/game/potion.hpp"
#include "shared/audio/constants.hpp"
#include "shared/utilities/rng.hpp"

Python::Python(glm::vec3 corner, glm::vec3 facing) :
    Enemy(corner, facing, ObjectType::Python, ModelType::Cube, SharedStats(
        Stat(0, 15, 15),
        Stat(0, 5, 2)
    ))
{
    this->last_move_time = std::chrono::system_clock::now();
    this->moveDelay = 3;
    this->moveDuration = 1;
    this->diagonal = false;

    this->physics.velocityMultiplier.y = 0;
    this->physics.velocityMultiplier.x = 0.4;
    this->physics.velocityMultiplier.z = 0.4;
    this->physics.shared.dimensions = glm::vec3(2.0f, 3.0f, 2.0f);
}

bool Python::doBehavior(ServerGameState& state) {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds{ now - this->last_move_time };
    
    if (elapsed_seconds > std::chrono::seconds(this->moveDelay)) {
        auto players = state.objects.getPlayers();
        float closest_dist = std::numeric_limits<float>::max();
        Player* target = nullptr;
        for (int p = 0; p < players.size(); p++) {
            auto player = players.get(p);
            if (player == nullptr) continue;
            if (!player->canBeTargetted()) continue;

            float distance_to_player = glm::distance(this->physics.shared.corner, player->physics.shared.corner);
            if (distance_to_player < closest_dist) {
                closest_dist = distance_to_player;
                target = player;
            }
        }

        if (closest_dist < Grid::grid_cell_width * Python::SIGHT_LIMIT_GRID_CELLS) {
            this->physics.shared.facing = glm::normalize(
                target->physics.shared.getCenterPosition() - this->physics.shared.getCenterPosition()
            );
        }
        else {
            this->physics.shared.facing = glm::normalize(glm::vec3(
                randomDouble(-0.5, 0.5), 0, randomDouble(-0.5, 0.5)
            ));
        }

        if (this->diagonal) {
            if (randomInt(0, 1) == 0) {
                this->physics.velocity.x = (this->physics.shared.facing.x * 0.5) * 0.525
                    + (this->physics.shared.facing.z * 0.5) * 0.85;
                this->physics.velocity.z = (this->physics.shared.facing.x * 0.5) * -0.85
                    + (this->physics.shared.facing.z * 0.5) * 0.525;
            }
            else {
                this->physics.velocity.x = (this->physics.shared.facing.x * 0.5) * 0.525
                    + (this->physics.shared.facing.z * 0.5) * -0.85;
                this->physics.velocity.z = (this->physics.shared.facing.x * 0.5) * 0.85
                    + (this->physics.shared.facing.z * 0.5) * 0.525;
            }
            this->diagonal = false;
        }
        else {
            this->physics.velocity.x = this->physics.shared.facing.x * 0.5;
            this->physics.velocity.z = this->physics.shared.facing.z * 0.5;
            this->diagonal = true;
        }
        
        this->last_move_time = now;

        return true;
    } 
    else if (elapsed_seconds > std::chrono::seconds(this->moveDuration)) {
        this->physics.velocity.x = 0;
        this->physics.velocity.z = 0;
        return true;
    }
        
    return false;
}

void Python::doCollision(Object* other, ServerGameState& state) {
    Creature* creature = dynamic_cast<Creature*>(other);
    if (creature == nullptr) return;

    if (creature->type == ObjectType::Player) {
        creature->stats.health.decrease(2);
        creature->physics.velocity = 0.5f * glm::normalize(this->physics.shared.facing);
    }
}

bool Python::doDeath(ServerGameState& state) {
    Enemy::doDeath(state);

    // Drop health potion upon death
    auto newCorner = this->physics.shared.corner;
    newCorner.y *= 0;

    auto rand = randomInt(1, 4);
    if (rand == 1) {
        state.objects.createObject(new Potion(newCorner, glm::vec3(1), PotionType::Nausea));
    }
    else if (rand == 4) {
        state.objects.createObject(new Potion(newCorner, glm::vec3(1), PotionType::Invincibility));
    }

    return true;
}