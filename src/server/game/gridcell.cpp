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
	case 'E':
		return CellType::Enemy;
	case 'X':
		return CellType::SpikeTrap;
	case 'P':
		return CellType::Potion;
	default:
		return CellType::Unknown;
	}
}