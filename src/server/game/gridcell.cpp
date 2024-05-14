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
	case '&':
		return CellType::FireballTrap;
	case '+':
		return CellType::FloorSpikeFull;
	case '|':
		return CellType::FloorSpikeVertical;
	case '-':
		return CellType::FloorSpikeHorizontal;
	case 'F':
		return CellType::FakeWall;
	case '^':
		return CellType::ArrowTrapUp;
	case 'v':
		return CellType::ArrowTrapDown;
	case '>':
		return CellType::ArrowTrapRight;
	case '<':
		return CellType::ArrowTrapLeft;
	case 'h':
		return CellType::HealthPotion;
	case 'n':
		return CellType::NauseaPotion;
	case 'i':
		return CellType::InvisibilityPotion;
	default:
		return CellType::Unknown;
	}
}