#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "Cooking/CookingTypes.h"
#include "ReactionDataAsset.generated.h"

// A single emergent reaction that fires when the right food tag combination
// is present across any dish at meal-eat time.
// Examples: Dairy + Acid = Curdled, Protein + Heat + Fat = Maillard, etc.
UCLASS(BlueprintType)
class CAPS_API UReactionDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction")
	FText ReactionName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction")
	FText Description;

	// All of these tags must be present across the meal for the reaction to fire.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction|Trigger")
	FGameplayTagContainer RequiredFoodTags;

	// If any of these tags are present the reaction is suppressed.
	// Lets you define "Curdled doesn't fire if you also have heat" etc.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction|Trigger")
	FGameplayTagContainer BlockingFoodTags;

	// Optional: reaction only fires if the ingredient is in a specific dish.
	// Empty = fires regardless of which dish the tags come from.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction|Trigger")
	TArray<ECookingSlot> RequiredSlots;

	// The GAS effect applied to the player when this reaction fires.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction|Effect")
	TSubclassOf<UGameplayEffect> ReactionEffect;

	// Positive = buff/bonus, Negative = penalty. Used for recipe book display only.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction|Display")
	bool bIsPositive = true;

	// Shown in the recipe book once discovered.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction|Display")
	FText DiscoveryHint;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Reaction", GetFName());
	}
};
