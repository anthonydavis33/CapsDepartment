#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GameplayEffect.h"
#include "Cooking/CookingTypes.h"
#include "IngredientDataAsset.generated.h"

// What an ingredient does when placed in a specific cooking slot.
// The same ingredient can have a completely different effect in MainDish vs Dessert.
USTRUCT(BlueprintType)
struct FIngredientSlotEffect
{
	GENERATED_BODY()

	// GAS effect applied to the player when this ingredient is in this slot.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> GameplayEffect;

	// Short description shown in the recipe book after first use.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText EffectHint;
};

UCLASS(BlueprintType)
class CAPS_API UIngredientDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ── Identity ──────────────────────────────────────────────────────────────

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ingredient")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ingredient")
	FText FlavorText;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ingredient")
	TObjectPtr<UTexture2D> Icon;

	// ── Slot Compatibility ────────────────────────────────────────────────────

	// Which sub-slot types this ingredient can occupy (e.g. Protein, Carb, Spice).
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ingredient|Slots")
	TArray<ESubSlotType> CompatibleSubSlots;

	// ── Effects Per Dish ──────────────────────────────────────────────────────

	// Keyed by which cooking slot (dish) this ingredient is placed into.
	// An ingredient with no entry for a slot does nothing in that slot.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ingredient|Effects")
	TMap<ECookingSlot, FIngredientSlotEffect> SlotEffects;

	// ── Reaction Tags ─────────────────────────────────────────────────────────

	// Food category tags used by the reaction resolver.
	// Define these in DefaultGameplayTags.ini under Food.Category.*
	// Examples: Food.Category.Dairy, Food.Category.Acid, Food.Category.Protein
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ingredient|Reactions")
	FGameplayTagContainer FoodTags;

	// ── Preparation System (post-MVP) ────────────────────────────────────────

	// If true, this ingredient cannot drop from enemies — it must be crafted
	// in the hub preparation queue (fridge/pantry). Loot tables check this flag.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ingredient|Preparation")
	bool bIsPreparedIngredient = false;

	// ── Future: Procedural Dish Display ───────────────────────────────────────

	// Broad category used to resolve dish archetype visuals (Protein, Carb, etc).
	// Will feed into the dish display tint system when that pass happens.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ingredient|Display")
	FGameplayTag DisplayCategory;

	// ── UPrimaryDataAsset ─────────────────────────────────────────────────────

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId("Ingredient", GetFName());
	}
};
