#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Core/CapsGameSettings.h"
#include "CapsRoomTileComponent.generated.h"

// Add to any tile actor placed inside a Level Instance room.
// SlotType must match a key in UCapsTilesetDataAsset::TileMeshes
// (e.g. "wall", "corner", "floor", "ceiling").
//
// ACapsRoomTemplate::ApplyTileset() finds all actors in the room with this
// component and swaps their UStaticMeshComponent mesh + material accordingly.
UCLASS(ClassGroup=Caps, meta=(BlueprintSpawnableComponent))
class CAPS_API UCapsRoomTileComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCapsRoomTileComponent();

	// Must match a key in the active tileset. Choices are defined in
	// Project Settings → Caps Dept → Tile Slot Types.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tile",
		meta=(GetOptions="GetSlotTypeOptions"))
	FName SlotType;

	UFUNCTION()
	TArray<FString> GetSlotTypeOptions() const;
};
