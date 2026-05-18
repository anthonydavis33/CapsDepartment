#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "CapsRoomTypes.generated.h"

UENUM(BlueprintType)
enum class ERoomType : uint8
{
	Combat   UMETA(DisplayName="Combat"),
	Treasure UMETA(DisplayName="Treasure"),
	Rest     UMETA(DisplayName="Rest"),
	Safe     UMETA(DisplayName="Safe"),  // Contains extraction trigger. No reinforcements.
	Hub      UMETA(DisplayName="Hub"),   // Starting area. Always visible on minimap, sealed once run begins.
};

UENUM(BlueprintType)
enum class ECardinalDirection : uint8
{
	North	UMETA(DisplayName="North"),
	East	UMETA(DisplayName="East"),
	South	UMETA(DisplayName="South"),
	West	UMETA(DisplayName="West"),
};

UENUM(BlueprintType)
enum class EDoorState : uint8
{
	Open	UMETA(DisplayName="Open"),
	Locked	UMETA(DisplayName="Locked"),   // Condition-gated (future use)
	Sealed	UMETA(DisplayName="Sealed"),   // Permanently closed — no path here
};

// Returns the opposite cardinal direction (North<->South, East<->West).
inline ECardinalDirection OppositeDirection(ECardinalDirection Dir)
{
	switch (Dir)
	{
	case ECardinalDirection::North: return ECardinalDirection::South;
	case ECardinalDirection::South: return ECardinalDirection::North;
	case ECardinalDirection::East:  return ECardinalDirection::West;
	case ECardinalDirection::West:  return ECardinalDirection::East;
	}
	return ECardinalDirection::North;
}

// World-space offset from a room center to that room's door position.
// RoomSize = full room width/height in Unreal units.
inline FVector DoorLocalOffset(ECardinalDirection Dir, float RoomSize)
{
	const float Half = RoomSize * 0.5f;
	switch (Dir)
	{
	case ECardinalDirection::North: return FVector( Half,    0.f, 0.f);
	case ECardinalDirection::South: return FVector(-Half,    0.f, 0.f);
	case ECardinalDirection::East:  return FVector(  0.f,  Half, 0.f);
	case ECardinalDirection::West:  return FVector(  0.f, -Half, 0.f);
	}
	return FVector::ZeroVector;
}

// ── Minimap ───────────────────────────────────────────────────────────────────

// One entry per room known to the minimap system.
// Rooms the player hasn't visited are excluded unless bRevealedByProximity is set
// (safe rooms within UCapsGameSettings::SafeRoomRevealRange tiles are always shown).
USTRUCT(BlueprintType)
struct FMinimapRoomEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly) int32 RoomIndex = 0;
	UPROPERTY(BlueprintReadOnly) ERoomType RoomType = ERoomType::Combat;
	UPROPERTY(BlueprintReadOnly) bool bVisited = false;
	UPROPERTY(BlueprintReadOnly) bool bIsCurrent = false;

	// True when this is an unvisited safe room within the reveal range.
	UPROPERTY(BlueprintReadOnly) bool bRevealedByProximity = false;

	// Live enemy positions in this room, normalized to [-1, 1] relative to the room center.
	// X = left/right across the room, Y = forward/back. Draw as red dots on the cell.
	UPROPERTY(BlueprintReadOnly) TArray<FVector2D> EnemyPositions;
};

// ── Dungeon Scaling ───────────────────────────────────────────────────────────

// One difficulty step. Tiers are sorted by StartAtRoom and applied in order.
USTRUCT(BlueprintType)
struct FDungeonScalingTier
{
	GENERATED_BODY()

	// Rooms completed before this tier activates (0 = from the start).
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 StartAtRoom = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HealthMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float DamageMultiplier = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float SpeedMultiplier = 1.f;

	// GAS effects applied to every enemy spawned in this tier and beyond.
	// Use for milestone buffs (e.g. "all enemies enrage after room 5").
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<TSubclassOf<UGameplayEffect>> MilestoneBuffEffects;
};
