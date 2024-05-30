#pragma once

#include "shared/game/sharedobject.hpp"
#include "shared/utilities/typedefs.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>

class ServerGameState;
class Item;

/**
 * Spawner Class manages the number of enemies in the maze
 *
 * Only the enemy value cap has to be set in constants.hpp for 
 * managing the max number of enemies to spawn
 */
class Spawner {
public:
    Item* dummyItem;

    Spawner();

    /*
     * General spawn method to manage number of enemies
     */
    void spawn(ServerGameState& state);

    /*
     * Selects and spawns the enemy with amount of value remaining
     */
    void spawnEnemy(ServerGameState& state, int valueRemaining);

    /*
     * Decreases the enemy value when enemies are killed/dead
     */
    void decreaseValue(SpecificID id);

    /*
     * Method to manually add enemies for tracking if not spawned by spawner
     * e.g slime creating its own clone
     */
    void addEnemy(ServerGameState& state, SpecificID id);

    /*
     * Find empty positions that enemies can safely spawn
     */
    glm::vec3 findEmptyPosition(ServerGameState& state);

    /*
     * General spawn method to manage number of enemies
     */
    void spawnDummy(ServerGameState& state);


private:
    int enemyValueCap;
    int currentEnemyValue;
    std::vector<int> valueMap;
    std::unordered_map<SpecificID, int> enemiesAlive;
};
