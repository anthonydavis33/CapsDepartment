#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CapsTilesetDataAsset.generated.h"

// One visual skin for dungeon rooms. Maps named tile slots to static meshes
// and optional material overrides.
//
// Slot names (e.g. "wall", "corner", "floor") must match the SlotType set on
// UCapsRoomTileComponent instances placed inside each Level Instance room.
// Any slot not defined in this asset is left unchanged on the mesh actor.
UCLASS(BlueprintType)
class CAPS_API UCapsTilesetDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	// Mesh swapped onto tile actors whose SlotType matches the key.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tileset")
	TMap<FName, TObjectPtr<UStaticMesh>> TileMeshes;

	// Material applied to element 0 of the mesh after the swap (optional, per slot).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tileset")
	TMap<FName, TObjectPtr<UMaterialInterface>> TileMaterials;

	// Mesh applied to ACapsDoorActor instances in rooms using this tileset.
	// Also used as the exit-door preview on the previous room.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Tileset")
	TObjectPtr<UStaticMesh> DoorMesh;

	UFUNCTION(BlueprintCallable, Category="Tileset")
	UStaticMesh* GetMeshForSlot(FName SlotName) const;

	UFUNCTION(BlueprintCallable, Category="Tileset")
	UMaterialInterface* GetMaterialForSlot(FName SlotName) const;
};
