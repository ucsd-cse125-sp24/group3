#include "server/game/object.hpp"
#include "server/game/constants.hpp"
#include "server/game/arrowtrap.hpp"
#include "server/game/spell.hpp"

#include <optional>
#include <deque>
#include <iostream>

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
         * @param target Target towards which the projectile homes, if it is homing
         */
        Options(bool isSpell, int damage, float h_mult, float v_mult,
            bool homing, float homing_strength, 
            std::optional<EntityID> target
        ):
            isSpell(isSpell), damage(damage), h_mult(h_mult), 
            v_mult(v_mult), homing(homing), homing_strength(homing_strength),
            target(target)
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

private:
    Options opt;
    std::optional<ServerSFX> destroy_sound;
};

class HomingFireball : public Projectile {
public:
    inline static const int DAMAGE = 15;
    inline static const float H_MULT = 0.4;
    inline static const float V_MULT = 0.0; // not affected by gravity
    inline static const float HOMING_STRENGTH = 0.1f;

    HomingFireball(glm::vec3 corner, glm::vec3 facing, std::optional<EntityID> target):
        Projectile(corner, facing, glm::vec3(0.4f, 0.4f, 0.4f), ModelType::Cube, ServerSFX::FireballImpact,
            Options(false, DAMAGE, H_MULT, V_MULT, true, HOMING_STRENGTH, target))
    {}
};

/**
 * TODO: this currently only supports UP, DOWN, LEFT, RIGHT directions.
 * Once we get an arrow model and can rotate models we can drop the dir
 * parameter and strictly use the facing parameter
 */
class Arrow : public Projectile {
public:
    inline static const int DAMAGE = 10;
    inline static const float H_MULT = 1.20f;
    inline static const float V_MULT = 0.0f; // not affected by gravity

    Arrow(glm::vec3 corner, glm::vec3 facing, Direction dir):
        Projectile(corner, facing, glm::vec3(0.0f, 0.0f, 0.0f), ModelType::Cube, ServerSFX::ArrowImpact,
            Options(false, DAMAGE, H_MULT, V_MULT, false, 0.0f, {}))
    {
        // temp hack to get the correct direction until we load in a model and can rotate it

        const float ARROW_WIDTH = 0.2f;    
        const float ARROW_LENGTH = 1.0f;    
        const float ARROW_HEIGHT = 0.2f;

        float arrow_x_dim;
        float arrow_z_dim;

        switch (dir) {
            case Direction::UP:
            case Direction::DOWN:
                arrow_x_dim = ARROW_WIDTH;
                arrow_z_dim = ARROW_LENGTH;
                break;
            case Direction::LEFT:
            case Direction::RIGHT:
                arrow_x_dim = ARROW_LENGTH;
                arrow_z_dim = ARROW_WIDTH;
                break;
        }

        this->physics.shared.dimensions = glm::vec3(arrow_x_dim, ARROW_HEIGHT, arrow_z_dim);
    }
};

class SpellOrb : public Projectile {
public:
    inline static const int DAMAGE = 25;
    inline static const float H_MULT = 0.4;
    inline static const float V_MULT = 0.0;
    inline static const float HOMING_STRENGTH = 0.1f;
    
    SpellType sType;

    SpellOrb(glm::vec3 corner, glm::vec3 facing, SpellType type) :
        Projectile(corner, facing, glm::vec3(0.4f, 0.4f, 0.4f), ModelType::Cube, ServerSFX::FireballImpact,
            Options(true, DAMAGE, H_MULT, V_MULT, false, 0.0f, {}))
    {
        this->sType = type;
    }
};
