#include "server/game/enemy.hpp"
#include <chrono>

using namespace std::chrono_literals;

class Minotaur : public Enemy {
public:
    inline static const float SIGHT_LIMIT_GRID_CELLS = 10.0f;

    Minotaur(glm::vec3 corner, glm::vec3 facing);

    bool doBehavior(ServerGameState& state) override;

    void doCollision(Object* other, ServerGameState& state) override;

    bool doDeath(ServerGameState& state) override;

private:
    std::chrono::system_clock::time_point last_charge_time;
    int chargeDelay;
    int chargeDuration;
    bool stopped;
};
