#include "server/game/grid.hpp"
#include "shared/utilities/rng.hpp"
#include <iostream>

/*	Constructors and Destructors	*/
Grid::Grid(int rows, int columns) : rows(rows), columns(columns) {
	//	Note that there is a max columns limit.
	assert(columns <= MAX_MAZE_COLUMNS);

	//	Intialize GridCell vector with specified number of rows and columns
	this->grid.resize(rows);

	for (int i = 0; i < rows; i++) {
		this->grid.at(i).resize(columns);
	}
}

Grid::~Grid() {}

/*	GridCell CRUD	*/

void Grid::addCell(int x, int y, CellType type) {
	//	Create a new GridCell with the given values
	GridCell* cell = new GridCell(x, y, type);

	//	Check whether the x and y coordinates are in range
	assert(0 <= x && x < columns);
	assert(0 <= y && y < rows);

	//	Check that the given cell is empty
	assert(this->grid.at(y).at(x) == nullptr);

	//	Add new cell
	this->grid.at(y).at(x) = cell;

	//	Add cell to list of spawn GridCells if it is of type Spawn
	if (cell->type == CellType::Spawn) {
		this->spawnCells.push_back(cell);
	}
}

GridCell* Grid::getCell(int x, int y) {
	//	Attempt to retrieve a GridCell at coordinates (x, y)

	//	Assert that input coordinates are within the Grid's dimensions
	assert(x >= 0 && x < columns && y >= 0 && y < rows);

	return this->grid.at(y).at(x);
}

/*	Getters and Setters	*/

int Grid::getRows() const {
	return this->rows;
}

int Grid::getColumns() const {
	return this->columns;
}

std::vector<std::vector<GridCell*>> Grid::getGrid() {
	return this->grid;
}

std::vector<GridCell*> Grid::getSpawnPoints() {
	return this->spawnCells;
}

glm::vec3 Grid::getRandomSpawnPoint() {
    //  TODO: Possibly replace this random spawn point with player assignments?
    //  I.e., assign each player a spawn point to avoid multiple players getting
    //  the same spawn point?
    size_t randomSpawnIndex = randomInt(0, this->spawnCells.size() - 1);
	return this->gridCellCenterPosition(this->spawnCells.at(randomSpawnIndex));
}

glm::vec3 Grid::gridCellCenterPosition(GridCell* cell) {
	if (cell == nullptr) {
		return glm::vec3(0, 0, 0);
	}

	//	Compute cell's center position
	return glm::vec3((0.5 + cell->x) * grid_cell_width,
			0,
			(0.5 + cell->y) * grid_cell_width);
}

/*	Static members	*/

//	Initialize GridCell width to default value
const float Grid::grid_cell_width = DEFAULT_GRIDCELL_WIDTH;

glm::ivec2 Grid::getGridCellFromPosition(glm::vec3 position) {
	//	Consider a GridCell with 2-D Grid coordinates (x, y).
	//	A 3-D game world position (x_w, y_w, z_w) is within that GridCell 
	//	iff
	//	x * grid_cell_width <= x_w < (x + 1) * grid_cell_width
	//	and
	//	y * grid_cell_width <= z_w < (y + 1) * grid_cell_width
	//
	//	Therefore, given a 3-D game world position (x_w, y_w, z_w), we can
	//	determine the coordinates of the GridCell containing it via
	//	x = floor(x_w / grid_cell_width) and y = floor(z_w / grid_cell_width)

	return glm::ivec2(glm::floor(position.x / grid_cell_width),
		glm::floor(position.z / grid_cell_width));
}

std::vector<glm::ivec2> Grid::getCellsFromPositionRange(glm::vec3 p1, glm::vec3 p2) {
	std::vector<glm::ivec2> cellPositions;
	//	Get GridCell positions for p1 and p2
	glm::ivec2 gridCellStart = Grid::getGridCellFromPosition(p1);
	glm::ivec2 gridCellEnd = Grid::getGridCellFromPosition(p2);

	/*std::cout << "First grid cell: " << glm::to_string(gridCellStart) << " ";
	std::cout << "Second grid cell: " << glm::to_string(gridCellEnd) << std::endl;*/

	if (gridCellStart.x > gridCellEnd.x || gridCellStart.y > gridCellEnd.y) {
		return std::vector<glm::ivec2>();
	}

	if (gridCellStart == gridCellEnd) {
		return std::vector<glm::ivec2> { gridCellStart };
	}

	for (int x = gridCellStart.x; x <= gridCellEnd.x; x++) {
		for (int y = gridCellStart.y; y <= gridCellEnd.y; y++) {
			cellPositions.push_back(glm::ivec2(x, y));
		}
	}

	//std::cout << "cellPositions length: " << cellPositions.size() << std::endl;

	return cellPositions;
}