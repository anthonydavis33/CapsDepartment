#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Cooking/CookingTypes.h"
#include "CookingSlotDefinition.generated.h"

// Describes one sub-slot within a dish (e.g. the Protein slot in the Main Dish).
USTRUCT(BlueprintType)
struct FSubSlotDefinition
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESubSlotType SubSlotType;

	// How many ingredients can occupy this sub-slot (usually 1, 4 for Spice).
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 MaxIngredients = 1;

	// Display label shown in the cooking UI.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Label;
};

// Data asset that defines the sub-slot layout for one cooking station slot (dish).
// One asset per ECookingSlot. Assign in the CookingStationActor or a config asset.
//
// Layouts defined here:
//   MainDish  — Protein, Carb, Extra, Topping, Spice×4
//   Appetizer — Protein, Carb, Topping, Spice×4
//   Dessert   — Base, Sweetener, Texture, Garnish
//   Side      — Base, Spice×2
//   Drink     — Liquid, Flavor, Enhancement
UCLASS(BlueprintType)
class CAPS_API UCookingSlotDefinition : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slot")
	ECookingSlot SlotType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slot")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Slot")
	TArray<FSubSlotDefinition> SubSlots;
};
