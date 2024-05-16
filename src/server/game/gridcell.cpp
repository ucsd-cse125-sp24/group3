#include "server/game/gridcell.hpp"
#include <iostream>

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
	case 'p':
		return CellType::RandomPotion;
	case 's':
		return CellType::RandomSpell;
	case '*':
		return CellType::Orb;
	case ' ':
		return CellType::OutsideTheMaze;
	case 'T':
		return CellType::TeleporterTrap;
	default:
		std::cerr << "Unknown cell type: " << c << "\n";
		return CellType::Unknown;
	}
}

char cellTypeToChar(CellType type) {
	switch (type) {
	case CellType::Empty:
		return '.';
	case CellType::Wall:
		return '#';
	case CellType::Spawn:
		return '@';
	case CellType::Enemy:
		return 'E';
	case CellType::SpikeTrap:
		return 'X';
	case CellType::FireballTrap:
		return '&';
	case CellType::FloorSpikeFull:
		return '+';
	case CellType::FloorSpikeVertical:
		return '|';
	case CellType::FloorSpikeHorizontal:
		return '-';
	case CellType::FakeWall:
		return 'F';
	case CellType::ArrowTrapUp:
		return '^';
	case CellType::ArrowTrapDown:
		return 'v';
	case CellType::ArrowTrapRight:
		return '>';
	case CellType::ArrowTrapLeft:
		return '<';
	case CellType::HealthPotion:
		return 'h';
	case CellType::NauseaPotion:
		return 'n';
	case CellType::InvisibilityPotion:
		return 'i';
	case CellType::RandomPotion:
		return 'p';
	case CellType::RandomSpell:
		return 's';
	case CellType::Orb:
		return '*';
	case CellType::OutsideTheMaze:
		return ' ';
	case CellType::TeleporterTrap:
		return 'T';
	default:
		return '?';
	}
}

bool isCellTypeAllowedInEntryWay(CellType type) {
	return (type == CellType::Empty || type == CellType::FakeWall);
}