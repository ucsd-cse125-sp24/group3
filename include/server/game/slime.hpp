#include "server/game/enemy.hpp"
#include <chrono>

using namespace std::chrono_literals;

class Slime : public Enemy {
public:
    inline static std::chrono::seconds JUMP_INTERVAL = 2s;

    Slime(glm::vec3 corner, glm::vec3 facing, int size);

    bool doBehavior(ServerGameState& state) override;

    void doCollision(Object* other, ServerGameState& state) override;

private:
    std::chrono::system_clock::time_point last_jump_time;
};
