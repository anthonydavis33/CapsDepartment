#include "Core/CapsGameSettings.h"

UCapsGameSettings::UCapsGameSettings()
{
	// Default slot vocabulary. These are the constructor defaults — once a
	// designer saves from Project Settings the values live in DefaultGame.ini
	// and these are ignored. Add new slots there, not here.
	TileSlotTypes =
	{
		"wall",
		"corner",
		"floor",
		"ceiling",
		"pillar",
		"trim",
		"door_frame",
	};
}
