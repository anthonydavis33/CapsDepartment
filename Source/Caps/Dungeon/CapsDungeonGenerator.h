#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dungeon/CapsRoomTypes.h"
#include "CapsDungeonGenerator.generated.h"

class ACapsRoomTemplate;
class UDungeonScalingDataAsset;
class UCapsTilesetDataAsset;
class UCapsReinforcementComponent;
class ULevelStreamingDynamic;

// Placed once in the hub level. Streams Level Instance rooms into a linear
// East-extending chain, manages door states, and applies difficulty scaling.
//
// Hub wiring: call StartRun() when the player walks through the hub's West door.
// Room templates call NotifyRoomCleared() from Blueprint when all enemies die.
UCLASS()
class CAPS_API ACapsDungeonGenerator : public AActor
{
	GENERATED_BODY()

public:
	ACapsDungeonGenerator();

	// Level Instance packages to draw rooms from. Order is randomised each run.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dungeon")
	TArray<TSoftObjectPtr<UWorld>> RoomPool;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dungeon")
	TObjectPtr<UDungeonScalingDataAsset> ScalingData;

	// Tileset used when no other override is in effect.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Dungeon")
	TObjectPtr<UCapsTilesetDataAsset> DefaultTileset;

	// Switch tilesets mid-run. Updates the current room's exit door immediately
	// so the player sees the new theme before walking through.
	UFUNCTION(BlueprintCallable, Category="Dungeon")
	void SetTileset(UCapsTilesetDataAsset* NewTileset);

	// World-space distance between adjacent room centers (Unreal units).
	UPROPERTY(EditAnywhere, Category="Dungeon")
	float RoomSpacing = 5000.f;

	// Rooms to keep loaded on each side of the player's current room.
	UPROPERTY(EditAnywhere, Category="Dungeon")
	int32 StreamRadius = 1;

	// Begin a new run. Safe to call multiple times (resets all state).
	UFUNCTION(BlueprintCallable, Category="Dungeon")
	void StartRun();

	// Returns the difficulty tier active at the current room count.
	UFUNCTION(BlueprintCallable, Category="Dungeon")
	const FDungeonScalingTier& GetCurrentScalingTier() const;

	int32 GetRoomsCompleted() const { return RoomsCompleted; }

	// Returns the room template the player is currently in, or nullptr.
	ACapsRoomTemplate* GetCurrentRoomTemplate() const;

	// Returns minimap state for all known rooms. Call from the minimap widget each frame.
	UFUNCTION(BlueprintCallable, Category="Minimap")
	TArray<FMinimapRoomEntry> GetMinimapData() const;

protected:
	virtual void BeginPlay() override;

private:
	int32 CurrentRoomIndex = -1;
	int32 RoomsCompleted   = 0;
	TArray<int32> PoolSequence;

	UPROPERTY()
	TObjectPtr<UCapsTilesetDataAsset> CurrentTileset;

	UPROPERTY(VisibleAnywhere, Category="Dungeon")
	TObjectPtr<UCapsReinforcementComponent> ReinforcementComponent;

	// Minimap tracking — populated as rooms are activated and visited.
	TMap<int32, ERoomType> KnownRoomTypes;
	TSet<int32> VisitedRooms;

	UPROPERTY()
	TMap<int32, TObjectPtr<ACapsRoomTemplate>> ActiveRoomTemplates;

	// Maps each in-flight streaming level to its target room index.
	// Not UPROPERTY — streaming levels are world-owned during loading.
	TArray<TTuple<ULevelStreamingDynamic*, int32>> PendingLoads;

	void ShufflePool();
	void LoadRoom(int32 RoomIndex);
	void UnloadRoom(int32 RoomIndex);
	void ActivateRoom(ACapsRoomTemplate* Template, int32 RoomIndex);
	void UpdateStreamedRooms();
	FVector GetRoomOrigin(int32 RoomIndex) const;

	UFUNCTION()
	void OnPlayerPassedThroughDoor(ECardinalDirection Direction);

	UFUNCTION()
	void OnRoomLevelLoaded();

	UFUNCTION()
	void OnRoomCleared_Internal();
};
