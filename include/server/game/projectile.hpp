#include "server/game/object.hpp"

class Projectile : public Object {
public:
    Projectile(glm::vec3 corner, glm::vec3 facing, ModelType model, int damage);

    void doCollision(Object* other, ServerGameState* state) override;

private:
    int damage;
};