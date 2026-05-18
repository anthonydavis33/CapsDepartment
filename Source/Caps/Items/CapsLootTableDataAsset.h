#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CapsLootTableDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FLootEntry
{
	GENERATED_BODY()

	// Must match the FName used as the PrimaryAssetId for a UIngredientDataAsset.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName IngredientID;

	// Relative probability weight. Higher = more likely to be selected.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="0.0"))
	float Weight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1"))
	int32 MinCount = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta=(ClampMin="1"))
	int32 MaxCount = 1;
};

// Assigned per enemy type. Rolled on death to produce ingredient drops.
UCLASS(BlueprintType)
class CAPS_API UCapsLootTableDataAsset : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Loot")
	TArray<FLootEntry> Entries;

	// How many weighted draws to make per kill.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Loot", meta=(ClampMin="0"))
	int32 MinDrops = 1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Loot", meta=(ClampMin="0"))
	int32 MaxDrops = 2;

	// Returns a flat list of ingredient IDs to spawn (may contain duplicates for count > 1).
	UFUNCTION(BlueprintCallable, Category="Loot")
	TArray<FName> RollDrops() const;
};
