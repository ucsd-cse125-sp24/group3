#include "server/game/grid.hpp"

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