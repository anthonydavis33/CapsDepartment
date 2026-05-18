#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dungeon/CapsRoomTypes.h"
#include "CapsRoomTemplate.generated.h"

class ACapsDoorActor;
class UCapsTilesetDataAsset;

// ERoomType is defined in CapsRoomTypes.h (included above via Dungeon/CapsRoomTypes.h)

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnRoomCleared);

// Placed once per Level Instance room. Designer assigns Doors (ACapsDoorActors
// in the same sublevel) and EnemySpawnPoints. The generator sets RoomIndex
// and calls ConfigureDoors before the room becomes active.
UCLASS()
class CAPS_API ACapsRoomTemplate : public AActor
{
	GENERATED_BODY()

public:
	ACapsRoomTemplate();

	UPROPERTY(BlueprintReadOnly, Category="Room")
	int32 RoomIndex = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Room")
	ERoomType RoomType = ERoomType::Combat;

	// Drag-assign all ACapsDoorActors placed in this Level Instance.
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category="Room")
	TArray<TObjectPtr<ACapsDoorActor>> Doors;

	// World-space transforms for enemy spawns within this room.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Room")
	TArray<FTransform> EnemySpawnPoints;

	// Broadcast via NotifyRoomCleared. Call from Blueprint when all enemies die.
	UPROPERTY(BlueprintAssignable, Category="Room")
	FOnRoomCleared OnRoomCleared;

	// Sets each door's state: Open if in OpenDirections, Sealed otherwise.
	UFUNCTION(BlueprintCallable, Category="Room")
	void ConfigureDoors(const TSet<ECardinalDirection>& OpenDirections);

	// Returns the door actor for the given direction, or nullptr if absent.
	UFUNCTION(BlueprintCallable, Category="Room")
	ACapsDoorActor* GetDoor(ECardinalDirection Direction) const;

	// Call from Blueprint (or C++) when all original enemies in this room are dead.
	UFUNCTION(BlueprintCallable, Category="Room")
	void NotifyRoomCleared();

	// Called by UCapsReinforcementComponent when a wave arrives. Seals the exit
	// and increments the active reinforcement count.
	void LockForReinforcements(int32 EnemyCount);

	// Called by UCapsReinforcementComponent when each reinforcement enemy dies.
	// Re-opens the exit once all are cleared and the room was already beaten.
	void OnReinforcementEnemyDied();

	bool WasCleared() const { return bWasCleared; }

	// Optional per-room tileset override. Set this on the RoomTemplate actor in
	// the Level Instance to force a specific theme regardless of generator state
	// (useful for special rooms like boss antechambers or treasure vaults).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Room")
	TObjectPtr<UCapsTilesetDataAsset> DefaultTileset;

	// Swaps all tile actor meshes in this room and updates all doors.
	// Called by the generator at activation; also callable from Blueprint.
	UFUNCTION(BlueprintCallable, Category="Room")
	void ApplyTileset(UCapsTilesetDataAsset* Tileset);

	// Implement in Blueprint for room-level visual responses to a theme change
	// (e.g. swapping decal sets, changing ambient particles or lighting).
	UFUNCTION(BlueprintImplementableEvent, Category="Room")
	void OnTilesetApplied(UCapsTilesetDataAsset* Tileset);

protected:
	virtual void BeginPlay() override;

private:
	bool bWasCleared = false;
	int32 ActiveReinforcementCount = 0;
};
