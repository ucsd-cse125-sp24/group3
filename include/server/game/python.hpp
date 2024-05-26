#include "server/game/enemy.hpp"
#include <chrono>

using namespace std::chrono_literals;

class Python : public Enemy {
public:
    inline static const float SIGHT_LIMIT_GRID_CELLS = 6.0f;

    Python(glm::vec3 corner, glm::vec3 facing);

    bool doBehavior(ServerGameState& state) override;

    void doCollision(Object* other, ServerGameState& state) override;

    bool doDeath(ServerGameState& state) override;

private:
    std::chrono::system_clock::time_point last_move_time;
    int moveDelay;
    int moveDuration;
    bool diagonal;
};
