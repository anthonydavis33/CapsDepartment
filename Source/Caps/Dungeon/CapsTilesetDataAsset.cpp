#include "Dungeon/CapsTilesetDataAsset.h"

UStaticMesh* UCapsTilesetDataAsset::GetMeshForSlot(FName SlotName) const
{
	const TObjectPtr<UStaticMesh>* Found = TileMeshes.Find(SlotName);
	return Found ? Found->Get() : nullptr;
}

UMaterialInterface* UCapsTilesetDataAsset::GetMaterialForSlot(FName SlotName) const
{
	const TObjectPtr<UMaterialInterface>* Found = TileMaterials.Find(SlotName);
	return Found ? Found->Get() : nullptr;
}
