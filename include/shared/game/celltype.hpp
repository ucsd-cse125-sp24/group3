#pragma once

enum class CellType {
	Empty,
	Wall,
	Spawn,
	Enemy,
	FakeWall,
	HealthPotion,
	NauseaPotion,
	InvisibilityPotion,
	InvincibilityPotion,
	FireSpell,
	HealSpell,
	RandomPotion,
	RandomSpell,
	Orb,
	OutsideTheMaze,
	FloorSpikeHorizontal,
	FloorSpikeVertical,
	FloorSpikeFull,
	ArrowTrapUp,
	ArrowTrapDown,
	ArrowTrapLeft,
	ArrowTrapRight,
	SpikeTrap,
	FireballTrap,
	TeleporterTrap,
	Exit,
	Unknown
};