#include "server/game/enemy.hpp"
#include <chrono>

using namespace std::chrono_literals;

class Slime : public Enemy {
public:

    Slime(glm::vec3 corner, glm::vec3 facing, int size);

    bool doBehavior(ServerGameState& state) override;
    
    bool doDeath(ServerGameState& state) override;

    void doCollision(Object* other, ServerGameState& state) override;

private:
    std::vector<std::chrono::milliseconds> JUMP_INTERVALS = {3000ms, 2200ms, 2200ms};
    std::vector<float> JUMP_STRENGTHS = {0.3f, 0.3f, 0.8f};

    std::chrono::system_clock::time_point last_jump_time;

    void increaseJumpIndex();
    std::size_t jump_index;
    int size;
};
