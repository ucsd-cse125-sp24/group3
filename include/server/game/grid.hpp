#pragma once

#include "server/game/gridcell.hpp"
#include "server/game/constants.hpp"
#include <vector>

class Grid {
public:
	/*	Constructors and Destructors	*/

	Grid() : Grid(0, 0) {}
	
	/**
	 * @brief Initializes this Grid with the specified number of rows and
	 * columns.
	 * @param rows Number of rows in the grid.
	 * @param columns Number of columns in the grid.
	 */
	Grid(int rows, int columns);
	~Grid();

	/*	GridCell CRUD	*/

	/**
	 * @brief Adds a GridCell of the specified type at the given location. If
	 * the type is CellType::Spawn, then a pointer to the newly allocated
	 * GridCell will be added to the spawnCells vector.
	 * Note: If a GridCell already exists at the given location, the GridCell
	 * will not be added. (This function asserts that the given location is 
	 * empty).
	 * Note: A GridCell will not be added if its x and y coordinates are outside
	 * of the integer range [0, columns - 1] and [0, rows - 1]. (This function
	 * asserts that the given x and y coordinates are within this range).
	 * @param x x coordinate of the new GridCell.
	 * @param y y coordinate of the new GridCell.
	 * @param type type of the GridCell to add to the grid.
	 */
	void addCell(int x, int y, CellType type);

	/**
	 * @brief Attempts to retrieve a GridCell at the specified location.
	 * Note: This function asserts if the coordinates are outside of the grid's
	 * dimensions.
	 * Note: This function assumes that all GridCells inside the grid's
	 * dimensions are initialized.
	 * @param x x coordinate of the GridCell to retrieve.
	 * @param y y coordinate of the GridCell to retrieve.
	 * @return Pointer to the GridCell at the specified coordinates.
	 */
	GridCell* getCell(int x, int y);

	/*	Getters and Setters	*/

	/**
	 * @brief Returns the number of rows in this Grid.
	 * @return Integer number of rows in this Grid.
	 */
	int getRows() const;

	/**
	 * @brief Returns the number of columns in this Grid.
	 * @return Integer number of columns in this Grid.
	 */
	int getColumns() const;
	
	/**
	 * @brief Returns the 2-D GridCell vector maintained by this Grid.
	 * @return 2-D GridCell vector maintained by this Grid.
	 */
	std::vector<std::vector<GridCell*>> getGrid();

	/**
	 * @brief Returns the vector of spawn point GridCells in this grid.
	 * @return Vector of spawn point GridCells in this grid.
	 */
	std::vector<GridCell*> getSpawnPoints();

	/**
	 * @brief randomly selects a spawn point
	 * @return corner coordinate of a randomly selected spawn point
	 */
	glm::vec3 getRandomSpawnPoint();

	/**
	 * @brief Returns the center position (as an Object position vector) of the
	 * given GridCell's center.
	 * @param cell GridCell whose center position will be returned as an
	 * Object position vector.
	 * @return Center position of the given grid cell as an Object Physics
	 * position vector, or (0,0,0) if the given cell pointer is nullptr
	 */
	glm::vec3 gridCellCenterPosition(GridCell * cell);

	/*	Static members	*/

	/**
	 * @brief The width, in meters, of each GridCell in this grid.
	 */
	static float const grid_cell_width;

	/**
	 * @brief Returns the position of the GridCell that would contain the given
	 * position.
	 * @param position 3-D Game world position 
	 * @return 2-D Grid position of the GridCell that would contain the given
	 * position.
	 * @note It's possible that there may be no GridCell in a particularly used
	 * Grid that has the returned coordinates (meaning the input position is out
	 * of bounds of a given maze).
	 */
	static glm::ivec2 getGridCellFromPosition(glm::vec3 position);

	/**
	 * @brief Returns a vector of positions of GridCells that contain the
	 * rectangle that extends from p1 to p2 
	 * (assumes p1.x <= p2.x and p1.z <= p2.z)
	 * @param p1 3-D game world point from which rectangle extands
	 * @param p2 3-D game world point to which rectangle extends
	 * @return vector of positions of GridCells that contain the rectangle that
	 * extends from p1 to p2. If p1.x > p2.x or p1.z > p2.z, returns an empty
	 * vector.
	 */
	static std::vector<glm::ivec2> getCellsFromPositionRange(glm::vec3 p1,
		glm::vec3 p2);

private:

	/**
	 * @brief Number of rows in the 2-D GridCell vector.
	 */
	int rows;

	/**
	 * @brief Number of columns in the 2-D GridCell vector.
	 */
	int columns;

	/**
	 * @brief 2-D vector of GridCells that is generated after reading an input
	 * maze file.
	 */
	std::vector<std::vector<GridCell *>> grid;
	
	/**
	 * @brief List of GridCells that are spawn points in the grid.
	 */
	std::vector<GridCell*> spawnCells;
};