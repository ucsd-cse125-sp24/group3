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