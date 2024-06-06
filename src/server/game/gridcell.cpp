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
    case '!':
		return CellType::Pillar;
	case '@':
		return CellType::Spawn;
	case 'E':
		return CellType::Enemy;
	case 'X':
		return CellType::SpikeTrap;
	case '1':
		return CellType::FireballTrapLeft;
	case '2':
		return CellType::FireballTrapRight;
	case '3':
		return CellType::FireballTrapDown;
	case '4':
		return CellType::FireballTrapUp;
	case '+':
		return CellType::LavaCross;
	case '|':
		return CellType::LavaVertical;
	case '-':
		return CellType::LavaHorizontal;
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
    case '[':
        return CellType::TorchDown;
    case ']':
        return CellType::TorchUp;
    case '{':
        return CellType::TorchRight;
    case '}':
        return CellType::TorchLeft;
	case 'z':
		return CellType::InvincibilityPotion;
	case 'f':
		return CellType::FireSpell;
	case 'e':
		return CellType::HealSpell;
	case 't':
		return CellType::TeleportSpell;
	case 'd':
		return CellType::Dagger;
	case 'r':
		return CellType::Sword;
	case 'm':
		return CellType::Hammer;
	case 'w':
		return CellType::RandomWeapon;
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
	case 'o':
		return CellType::Exit;
	case 'M':
		return CellType::Mirror;
	case '5':
		return CellType::FloorSpikeHorizontal;
	case '6':
		return CellType::FloorSpikeVertical;
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
	case CellType::Pillar:
		return '!';
	case CellType::Spawn:
		return '@';
	case CellType::Enemy:
		return 'E';
	case CellType::SpikeTrap:
		return 'X';
	case CellType::FireballTrapLeft:
		return '1';
	case CellType::FireballTrapRight:
		return '2';
	case CellType::FireballTrapDown:
		return '3';
	case CellType::FireballTrapUp:
		return '4';
	case CellType::LavaCross:
		return '+';
	case CellType::LavaVertical:
		return '|';
	case CellType::LavaHorizontal:
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
	case CellType::TorchDown:
		return '[';
	case CellType::TorchUp:
		return ']';
	case CellType::TorchRight:
		return '{';
	case CellType::TorchLeft:
		return '}';
	case CellType::InvincibilityPotion:
		return 'z';
	case CellType::FireSpell:
		return 'f';
	case CellType::HealSpell:
		return 'e';
	case CellType::TeleportSpell:
		return 't';
	case CellType::Dagger:
		return 'd';
	case CellType::Sword:
		return 'r';
	case CellType::Hammer:
		return 'm';
	case CellType::RandomWeapon:
		return 'w';
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
	case CellType::Exit:
		return 'o';
	case CellType::Mirror:
		return 'M';
	case CellType::FloorSpikeHorizontal:
		return '5';
	case CellType::FloorSpikeVertical:
		return '6';
	default:
		return '?';
	}
}

bool isCellTypeAllowedInEntryWay(CellType type) {
	return (type == CellType::Empty || type == CellType::FakeWall);
}

bool isWallLikeCell(CellType type) {
	return (
		// not fake wall, because we still want to place a floor under it
		type == CellType::Wall ||
		type == CellType::TorchDown ||
		type == CellType::TorchUp ||
		type == CellType::TorchRight ||
		type == CellType::TorchLeft ||
		type == CellType::Pillar
	);
}
