#include "server/game/spawner.hpp"
#include "server/game/slime.hpp"
#include "server/game/python.hpp"
#include "server/game/minotaur.hpp"
#include "server/game/servergamestate.hpp"
#include "shared/utilities/rng.hpp"

/*
 * When adding new enemies to spawner class:
 * 1) add the enemy value in the constructor
 * 2) Create a new case in spawnEnemy with the index from valueMap
 * 3) add it to addEnemy() method if needed
 */
Spawner::Spawner() {
    this->enemyValueCap = MAX_ENEMY_VALUE;
    this->currentEnemyValue = 0;

    this->valueMap.push_back(20); // 0: Big slime (size = 4)
    this->valueMap.push_back(10); // 1: Medium slime (size = 3)
    this->valueMap.push_back(5);  // 2: Small slime (size = 2)
	this->valueMap.push_back(2);  // 3: Mini slime (size = 1)
	this->valueMap.push_back(25); // 4: Minotaur
	this->valueMap.push_back(10); // 5: Python
}

void Spawner::spawn(ServerGameState& state) {
	valueLock.lock();
	auto val = this->currentEnemyValue;
	valueLock.unlock();

	if (val >= MAX_ENEMY_VALUE) { return; }

	auto valRemaining = this->enemyValueCap - val;

	// temp numbers, to tune later...
	// fill a portion of the cap immediately
	if (val < MAX_ENEMY_VALUE * 0.1) {
		spawnEnemy(state, valRemaining);
	}
	else {
		// 1/300 chance every 30ms -> expected spawn every 9s 
		if (randomInt(1, 300) == 1) {
			spawnEnemy(state, valRemaining);
		}
	}
}   

void Spawner::spawnEnemy(ServerGameState& state, int valueRemaining) {
	glm::vec3 spawnLocation = findEmptyPosition(state);

	int index = 0;
	// Get enemy that can fit within value
	while (true) {
		index = randomInt(0, valueMap.size()-1);

		// Dont spawn mini slimes
		if (index == 3) { continue; }
		if (this->valueMap[index] < valueRemaining) {
			break;
		}
	}

	SpecificID enemyID = 0;
	auto value = 0;
	switch (index) {
	case 0:
	case 1:
	case 2: {
		auto size = 4 - index;
		enemyID = state.objects.createObject(new Slime(
			spawnLocation,
			glm::vec3(1, 0, 1),
			size
		));
		if (size == 4) {
			value = valueMap[0];
		}
		else if (size == 3) {
			value = valueMap[1];
		}
		else {
			value = valueMap[2];
		}
		break;
	}
	case 4: {
		enemyID = state.objects.createObject(new Minotaur(
			spawnLocation,
			glm::vec3(1, 0, 1)
		));
		value = valueMap[4];
		break;
	}
	case 5: {
		enemyID = state.objects.createObject(new Python(
			spawnLocation,
			glm::vec3(1, 0, 1)
		));
		value = valueMap[5];
		break;
	}
	}
	
	this->enemiesAlive[enemyID] = value;
	valueLock.lock();
	this->currentEnemyValue += value;
	valueLock.unlock();
}

glm::vec3 Spawner::findEmptyPosition(ServerGameState& state) {
	// TODO 2: check that no collision in the cell you are spawning in

	auto& grid = state.getGrid();
	int cols = grid.getColumns();
	int rows = grid.getRows();

	glm::ivec2 random_cell;
	while (true) {
		random_cell.x = randomInt(0, cols - 1);
		random_cell.y = randomInt(0, rows - 1); // corresponds to z in the world

		if (grid.getCell(random_cell.x, random_cell.y)->type == CellType::Empty) {
			break;
		}
	}

	return glm::vec3(random_cell.x * Grid::grid_cell_width, 0, random_cell.y * Grid::grid_cell_width);
}

void Spawner::addEnemy(ServerGameState& state, SpecificID id) {
	auto enemy = state.objects.getEnemy(id);
	auto type = enemy->type;

	auto value = 0;
	switch (type) {
	case ObjectType::Slime:
		Slime* slime = dynamic_cast<Slime*>(enemy);
		if (slime->size == 4) {
			value = valueMap[0];
		}
		else if (slime->size == 3) {
			value = valueMap[1];
		}
		else if (slime->size == 2) {
			value = valueMap[2];
		}
		else {
			value = valueMap[3];
		}
		break;
	}

	this->enemiesAlive[id] = value;
	valueLock.lock();
	this->currentEnemyValue += value;
	valueLock.unlock();
}

void Spawner::decreaseValue(SpecificID id) {
	auto value = this->enemiesAlive[id];
	this->enemiesAlive.erase(id);
	valueLock.lock();
	this->currentEnemyValue -= value;
	valueLock.unlock();
}