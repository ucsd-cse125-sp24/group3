#include "server/game/object.hpp"
#include "server/game/constants.hpp"
#include "server/game/arrowtrap.hpp"

#include <optional>
#include <deque>
#include <iostream>

class Projectile : public Object {
public:
    struct Options {
        Options(int damage, float h_mult, float v_mult,
            bool disappear, bool homing, float homing_strength, 
            std::optional<EntityID> target
        ):
            damage(damage), h_mult(h_mult), v_mult(v_mult),
            disappearOnContact(disappear), homing(homing), homing_strength(homing_strength),
            target(target)
        {
            if (homing && !target.has_value()) {
                std::cerr << "FATAL: homing projectile created without target.\n"
                    << "We could potentially implement this to home on the closest Player / Enemy object,\n"
                    << "But this isn't implemented yet.\n";
                std::exit(1);
            }
        }

        float h_mult;
        float v_mult;
        int damage;
        bool disappearOnContact;
        bool homing;
        float homing_strength;
        std::optional<EntityID> target;
    };

    Projectile(glm::vec3 corner, glm::vec3 facing, 
        glm::vec3 dimensions, ModelType model,
        Options options);

    void doCollision(Object* other, ServerGameState* state) override;

    void doTick(ServerGameState* state) override;

private:
    void _addTargetPosLag(glm::vec3 target_pos);

    Options opt;
};

class HomingFireball : public Projectile {
public:
    static const int DAMAGE = 25;
    static const float H_MULT = 0.4;
    static const float V_MULT = 0.5;
    static const float HOMING_STRENGTH = 0.1f;

    HomingFireball(glm::vec3 corner, glm::vec3 facing, std::optional<EntityID> target):
        Projectile(corner, facing, glm::vec3(0.4f, 0.4f, 0.4f), ModelType::Cube,
            Options(DAMAGE, H_MULT, V_MULT, true, true, HOMING_STRENGTH, target))
    {}
};

class Arrow : public Projectile {
public:
    static const int DAMAGE = 10;
    static const float H_MULT = 1.20f;
    static const float V_MULT = 0.0f; // not affected by gravity

    Arrow(glm::vec3 corner, glm::vec3 facing, ArrowTrap::Direction dir):
        Projectile(corner, facing, glm::vec3(0.0f, 0.0f, 0.0f), ModelType::Cube,
            Options(DAMAGE, 
                H_MULT, V_MULT,
                false, false,
                0.0f,
                {}))
    {
        // temp hack to get the correct direction until we load in a model and can rotate it

        const float ARROW_WIDTH = 0.2f;    
        const float ARROW_LENGTH = 1.0f;    
        const float ARROW_HEIGHT = 0.2f;

        float arrow_x_dim;
        float arrow_z_dim;

        switch (dir) {
            case ArrowTrap::Direction::UP:
            case ArrowTrap::Direction::DOWN:
                arrow_x_dim = ARROW_WIDTH;
                arrow_z_dim = ARROW_LENGTH;
                break;
            case ArrowTrap::Direction::LEFT:
            case ArrowTrap::Direction::RIGHT:
                arrow_x_dim = ARROW_LENGTH;
                arrow_z_dim = ARROW_WIDTH;
                break;
        }

        this->physics.shared.dimensions = glm::vec3(arrow_x_dim, ARROW_HEIGHT, arrow_z_dim);
    }
};