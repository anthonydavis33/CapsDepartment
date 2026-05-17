#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "Cooking/CookingTypes.h"
#include "ReactionDataAsset.generated.h"

// A single emergent reaction that fires when the right food tag combination
// is present WITHIN A SINGLE DISH at meal-eat time.
// The resolver runs per-dish: dairy + acid both in the Main Dish = Curdled fires.
// Dairy in Main, acid in Appetizer = no reaction. Scope is intentional.
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

	// All of these tags must be present within the same dish for the reaction to fire.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction|Trigger")
	FGameplayTagContainer RequiredFoodTags;

	// If any of these tags are present in the same dish, the reaction is suppressed.
	// e.g. "Curdled doesn't fire if the dish also contains Heat."
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction|Trigger")
	FGameplayTagContainer BlockingFoodTags;

	// Which dish types this reaction can fire in. Empty = any dish.
	// Use this to restrict reactions that only make culinary sense in certain dishes
	// (e.g. a dessert caramelisation reaction shouldn't fire in a meat main).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reaction|Trigger")
	TArray<ECookingSlot> ApplicableDishTypes;

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
