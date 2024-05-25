#include "server/game/enemy.hpp"
#include <chrono>

using namespace std::chrono_literals;

class Slime : public Enemy {
public:
    inline static const float SIGHT_LIMIT_GRID_CELLS = 8.0f; // can see you within 8 grid cells

    Slime(glm::vec3 corner, glm::vec3 facing, int size);

    bool doBehavior(ServerGameState& state) override;
    
    bool doDeath(ServerGameState& state) override;

    void doCollision(Object* other, ServerGameState& state) override;

private:
    std::vector<std::chrono::milliseconds> jump_intervals;
    std::vector<float> jump_strengths;

    std::chrono::system_clock::time_point last_jump_time;

    void increaseJumpIndex();
    std::size_t jump_index;
    int size;

    bool landed;
};
