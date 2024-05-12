#include "server/game/object.hpp"

#include <deque>

class Projectile : public Object {
public:
    struct Options {
        Options(int damage, float h_mult, float v_mult, bool disappear, bool homing, float homing_strength, EntityID target):
            damage(damage), h_mult(h_mult), v_mult(v_mult),
            disappearOnContact(disappear), homing(homing), homing_strength(homing_strength), target(target) {}

        float h_mult;
        float v_mult;
        int damage;
        bool disappearOnContact;
        bool homing;
        float homing_strength;
        EntityID target;
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