#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Dungeon/CapsRoomTypes.h"
#include "DungeonScalingDataAsset.generated.h"

// Designer-configured difficulty curve for the infinite dungeon.
// Tiers must be sorted by StartAtRoom ascending; the first should have StartAtRoom = 0.
UCLASS(BlueprintType)
class CAPS_API UDungeonScalingDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Scaling")
	TArray<FDungeonScalingTier> Tiers;

	// Returns the active scaling tier for a given number of completed rooms.
	UFUNCTION(BlueprintCallable, Category="Scaling")
	const FDungeonScalingTier& GetTierForRoom(int32 RoomsCompleted) const;
};
