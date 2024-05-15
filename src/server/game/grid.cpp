#include "server/game/grid.hpp"
#include "shared/utilities/rng.hpp"
#include "shared/utilities/root_path.hpp"
#include <boost/filesystem.hpp>
#include <fstream>

/*	Constructors and Destructors	*/
Grid::Grid(int rows, int columns) : rows(rows), columns(columns) {
	//	Note that there is a max columns limit.
	assert(columns <= MAX_MAZE_COLUMNS);

	//	Initialize GridCell width to default value
	this->grid_cell_width = DEFAULT_GRIDCELL_WIDTH;

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
void Grid::setGridCellWidth(float new_width) {
	assert(new_width > 0);

	this->grid_cell_width = new_width;
}

float Grid::getGridCellWidth() const {
	return this->grid_cell_width;
}

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

void Grid::writeToFile() {
	boost::filesystem::path path = getRepoRoot() / "maps" / "generated" / "test.maze";	

	std::ofstream of;
	of.open(path.string());

	assert(of.is_open());

	for (int x = 0; x < this->columns; x++) {
		for (int y = 0; y < this->rows; y++) {
			CellType type = this->getCell(x, y)->type;
			of << cellTypeToChar(type);
		}
		of << '\n';
	}

	of.close();
}
