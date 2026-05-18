#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CapsBossModifierPoolDataAsset.generated.h"

class UCapsBossModifierDataAsset;

// Holds all available boss modifiers for a single boss or dungeon.
// Assign this on ACapsBossDoorActor to control what modifiers can appear.
//
// DrawModifiers() shuffles the pool and returns Count unique entries.
// Create one pool per boss (or share one globally for a single-boss MVP).
UCLASS(BlueprintType)
class CAPS_API UCapsBossModifierPoolDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Pool")
	TArray<TObjectPtr<UCapsBossModifierDataAsset>> Modifiers;

	// Randomly selects Count unique modifiers from the pool.
	// Returns fewer entries if the pool has fewer than Count valid assets.
	UFUNCTION(BlueprintCallable, Category="Boss")
	TArray<UCapsBossModifierDataAsset*> DrawModifiers(int32 Count) const;
};
