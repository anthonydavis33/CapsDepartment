#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "CapsGameSettings.generated.h"

// Project-wide designer settings. Edit via Project Settings → Caps Dept.
// Values persist in DefaultGame.ini — add new entries without recompiling.
//
// This is the intended home for any value that designers need to tune freely
// during development: slot type lists, tag vocabularies, tunable caps, etc.
UCLASS(Config=Game, defaultconfig, meta=(DisplayName="Caps Dept", CategoryName="Caps Dept"))
class CAPS_API UCapsGameSettings : public UDeveloperSettings
{
	GENERATED_BODY()

public:
	UCapsGameSettings();

	static const UCapsGameSettings* Get() { return GetDefault<UCapsGameSettings>(); }

	// ── Dungeon | Tiles ───────────────────────────────────────────────────────

	// Named tile slots shared between UCapsRoomTileComponent and UCapsTilesetDataAsset.
	// Add a new name here to make it available in the editor dropdown on tile actors
	// and as a valid key in tileset data assets — no code change required.
	UPROPERTY(Config, EditAnywhere, Category="Dungeon|Tiles")
	TArray<FName> TileSlotTypes;

	// ── Dungeon | Minimap ─────────────────────────────────────────────────────

	// How many rooms away an unvisited Safe room is revealed on the minimap.
	// Set to 0 to only show visited rooms.
	UPROPERTY(Config, EditAnywhere, Category="Dungeon|Minimap", meta=(ClampMin="0"))
	int32 SafeRoomRevealRange = 3;

	// ── Dungeon | Reinforcements ───────────────────────────────────────────────
	//
	// Each cleared room becomes a persistent wave source. Waves arrive at the
	// player's current room and seal the exit until cleared.

	// Seconds after a room clears before it sends its first wave.
	UPROPERTY(Config, EditAnywhere, Category="Dungeon|Reinforcements", meta=(ClampMin="5.0"))
	float ReinforcementInitialDelay = 60.f;

	// Seconds between subsequent waves from the same cleared room.
	UPROPERTY(Config, EditAnywhere, Category="Dungeon|Reinforcements", meta=(ClampMin="5.0"))
	float ReinforcementWaveInterval = 45.f;

	// 0.0–1.0 chance that a cleared room actually fires its wave when the timer expires.
	// Failed rolls reset the timer and try again next interval.
	UPROPERTY(Config, EditAnywhere, Category="Dungeon|Reinforcements", meta=(ClampMin="0.0", ClampMax="1.0"))
	float ReinforcementSpawnChance = 0.75f;

	// Minimum enemies in any single wave.
	UPROPERTY(Config, EditAnywhere, Category="Dungeon|Reinforcements", meta=(ClampMin="1"))
	int32 BaseReinforcementsPerWave = 1;

	// Additional enemies added per room cleared at the time the wave spawns.
	// Wave size = ceil(Base + RoomsCleared * ThisValue).
	UPROPERTY(Config, EditAnywhere, Category="Dungeon|Reinforcements", meta=(ClampMin="0.0"))
	float ReinforcementsPerRoomCleared = 0.25f;
};
