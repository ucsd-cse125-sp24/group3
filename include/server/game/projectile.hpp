#include "server/game/object.hpp"
#include "server/game/constants.hpp"
#include "server/game/arrowtrap.hpp"
#include "server/game/spell.hpp"
#include "shared/game/sharedmodel.hpp"

#include <optional>
#include <deque>
#include <iostream>
#include <variant>

/**
 * Class for any possible Projectile.
 * 
 * More specific projectiles are just different parameters being passed into this base class.
 * 
 * Potentially we might want more specific projectiles, at which time maybe we make some of the
 * projectile methods virtual.
 */
class Projectile : public Object {
public:
    /// @brief Struct for options to configure the projectile
    struct Options {
        /**
         * @param damage Damage when the projectile collides with a creature
         * @param h_mult horizontal velocity multiplier
         * @param v_mult vertical velocity multiplier
         * @param homing whether or not the projectile homes in on a specified target
         * @param homing_strength value from 0-1, where the closer to 1 the more strong the homing is
         * @param homing_duration how many ticks it should home for
         * @param target Target towards which the projectile homes, if it is homing
         */
        Options(bool isSpell, int damage, float h_mult, float v_mult,
            bool homing, float homing_strength, int homing_duration,
            std::optional<EntityID> target
        ):
            isSpell(isSpell), damage(damage), h_mult(h_mult), 
            v_mult(v_mult), homing(homing), homing_strength(homing_strength),
            homing_duration(homing_duration), target(target)
        {
            if (homing && !target.has_value()) {
                std::cerr << "FATAL: homing projectile created without target.\n"
                    << "We could potentially implement this to home on the closest Player / Enemy object,\n"
                    << "But this isn't implemented yet.\n";
                std::exit(1);
            }
        }

        bool isSpell;
        float h_mult;
        float v_mult;
        int damage;
        bool homing;
        float homing_strength;
        int homing_duration;
        std::optional<EntityID> target;
    };

    /**
     * @param corner Corner position of the projectile
     * @param facing Direction the projectile is facing
     * @param dimensions dimension of the projectile TODO: remove once actually using non Cube models
     * @param model Model to render the projectile as
     * @param options Configurable options for the projectile
     */
    Projectile(glm::vec3 corner, glm::vec3 facing, 
        glm::vec3 dimensions, ModelType model,
        std::optional<ServerSFX> destroy_sound, Options&& options);

    void doCollision(Object* other, ServerGameState& state) override;

	/**
     * @brief handle homing trajectory updates
	 * 
	 * @returns True if the entity was updated, false otherwise
	 */
    bool doTick(ServerGameState& state);

	virtual SharedObject toShared() override;


private:
    Options opt;
    std::optional<ServerSFX> destroy_sound;
    PointLightProperties properties;

};

class HomingFireball : public Projectile {
public:
    inline static const int DAMAGE = 15;
    inline static const float H_MULT = 0.4;
    inline static const float V_MULT = 0.1;
    inline static const float HOMING_STRENGTH = 0.1f;
    inline static const int HOMING_DURATION_TICKS = 80; // 2.4s

    HomingFireball(glm::vec3 corner, glm::vec3 facing, std::optional<EntityID> target):
        Projectile(corner, facing, glm::vec3(0.4f, 0.4f, 0.4f), ModelType::Fireball, ServerSFX::FireballImpact,
            Options(false, DAMAGE, H_MULT, V_MULT, true, HOMING_STRENGTH, HOMING_DURATION_TICKS, target))
    {
        this->physics.feels_gravity = false;
    }
};

/**
 * TODO: this currently only supports UP, DOWN, LEFT, RIGHT directions.
 * Once we get an arrow model and can rotate models we can drop the dir
 * parameter and strictly use the facing parameter
 */
class Arrow : public Projectile {
public:
    inline static const int DAMAGE = 10;
    inline static const float H_MULT = 0.55f;
    inline static const float V_MULT = 0.0f; // not affected by gravity

    Arrow(glm::vec3 corner, glm::vec3 facing, Direction dir):
        Projectile(corner, facing, glm::vec3(0.0f, 0.0f, 0.0f), ModelType::Arrow, ServerSFX::ArrowImpact,
            Options(false, DAMAGE, H_MULT, V_MULT, false, 0.0f, 0, {})), dir(dir)
    {
        const float clear_model_nudge = 1.5f;
        switch (dir) {
            case Direction::LEFT:
                std::swap(this->physics.shared.dimensions.x, this->physics.shared.dimensions.z);
                this->physics.shared.facing = glm::vec3(0.0f, 0.0f, -1.0f);
                this->physics.shared.corner.x -= clear_model_nudge;
                break;
            case Direction::RIGHT:
                std::swap(this->physics.shared.dimensions.x, this->physics.shared.dimensions.z);
                this->physics.shared.facing = glm::vec3(0.0f, 0.0f, 1.0f);
                // right doesn't need nudge for some reason
                // this->physics.shared.corner.x += clear_model_nudge;
                break;
            case Direction::UP:
                this->physics.shared.facing = glm::vec3(1.0f, 0.0f, 0.0f);
                this->physics.shared.corner.z -= (2.0f * clear_model_nudge);
                break;
            case Direction::DOWN:
                this->physics.shared.facing = glm::vec3(-1.0f, 0.0f, 0.0f);
                this->physics.shared.corner.z += clear_model_nudge;
                break;
        }
    }

    Direction dir;
};

class SpellOrb : public Projectile {
public:
    inline static const int DAMAGE = 25;
    inline static const float H_MULT = 0.4;
    inline static const float V_MULT = 0.0;
    inline static const float HOMING_STRENGTH = 0.1f;
    
    SpellType sType;

    SpellOrb(glm::vec3 corner, glm::vec3 facing, SpellType type) :
        Projectile(corner, facing, glm::vec3(0.4f, 0.4f, 0.4f), ModelType::SpellOrb, ServerSFX::FireballImpact,
            Options(true, DAMAGE, H_MULT, V_MULT, false, 0.0f, 0, {}))
    {
        this->sType = type;
    }
};
