#pragma once

enum class CellType {
	Empty,
	Wall,
	Spawn,
	Enemy,
	SpikeTrap,
	FireballTrap,
	FloorSpikeHorizontal,
	FloorSpikeVertical,
	FloorSpikeFull,
	FakeWall,
	ArrowTrapUp,
	ArrowTrapDown,
	ArrowTrapLeft,
	ArrowTrapRight,
	HealthPotion,
	NauseaPotion,
	InvisibilityPotion,
	Unknown
};

/**
 * @brief Returns CellType given a character (maps character representation to
 * CellType).
 * @param c Character to map to its corresponding CellType.
 * @return CellType represented by the given character, or CellType::Unknown if
 * the character doesn't map to a known CellType.
 */
CellType charToCellType(char c);

/**
 * TODO: write this
 */
char cellTypeToChar(CellType type);

bool isCellTypeAllowedInEntryWay(CellType type);

class GridCell {
public:
	/**
	 * @brief GridCell constructor.
	 * @param x x position of new GridCell
	 * @param y y position of new GridCell
	 * @param type CellType type of new GridCell
	 */
	GridCell(int x, int y, CellType type);
	int x;
	int y;
	CellType type;

	//	TODO: Possibly add a vec3 position vector here for the center of the
	//	GridCell?
};