#include "server/game/gridcell.hpp"

/*	Constructors and Destructors	*/
GridCell::GridCell(int x, int y, CellType type) : x(x), y(y), type(type) {}

CellType charToCellType(char c) {
	switch (c) {
	case '.':
		return CellType::Empty;
	case '#':
		return CellType::Wall;
	case '@':
		return CellType::Spawn;
	default:
		return CellType::Unknown;
	}
}