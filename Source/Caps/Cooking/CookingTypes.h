#pragma once

#include "CoreMinimal.h"
#include "CookingTypes.generated.h"

// The five cooking stations — one per player input.
UENUM(BlueprintType)
enum class ECookingSlot : uint8
{
	MainDish	UMETA(DisplayName="Main Dish (Attack 1)"),
	Appetizer	UMETA(DisplayName="Appetizer (Attack 2)"),
	Dessert		UMETA(DisplayName="Dessert (Dodge)"),
	Side		UMETA(DisplayName="Side (Movement)"),
	Drink		UMETA(DisplayName="Drink (Potion)"),
};

// The typed positions within each dish.
// Not every dish uses every sub-slot — see UCookingSlotDefinition.
UENUM(BlueprintType)
enum class ESubSlotType : uint8
{
	// Main Dish & Appetizer
	Protein		UMETA(DisplayName="Protein"),
	Carb		UMETA(DisplayName="Carb"),
	Extra		UMETA(DisplayName="Extra"),
	Topping		UMETA(DisplayName="Topping"),
	Spice		UMETA(DisplayName="Spice"),

	// Dessert
	Base		UMETA(DisplayName="Base"),
	Sweetener	UMETA(DisplayName="Sweetener"),
	Texture		UMETA(DisplayName="Texture"),
	Garnish		UMETA(DisplayName="Garnish"),

	// Drink
	Liquid		UMETA(DisplayName="Liquid"),
	Flavor		UMETA(DisplayName="Flavor"),
	Enhancement	UMETA(DisplayName="Enhancement"),
};

// Food category tags used by the reaction system.
// Applied as GameplayTags on ingredients — defined in DefaultGameplayTags.ini.
// Examples: Food.Category.Dairy, Food.Category.Acid, Food.Category.Citrus, etc.

// Quality tier of an ingredient. Maps directly to GAS GE Level (Table=1, Choice=2, Prime=3).
// Add this to loot table drops and pickup actors as you build those systems.
UENUM(BlueprintType)
enum class EIngredientQuality : uint8
{
	None   = 0  UMETA(DisplayName="None (unassigned)"),
	Table  = 1  UMETA(DisplayName="Table"),
	Choice = 2  UMETA(DisplayName="Choice"),
	Prime  = 3  UMETA(DisplayName="Prime"),
};

// Compound key used as the TMap key for inventory buckets (BaseStock, RunPickups).
// Combines IngredientID + Quality so Table Pepper and Prime Pepper are distinct stacks.
USTRUCT(BlueprintType)
struct FIngredientInstance
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName IngredientID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EIngredientQuality Quality = EIngredientQuality::Choice;

	bool operator==(const FIngredientInstance& Other) const
	{
		return IngredientID == Other.IngredientID && Quality == Other.Quality;
	}
};

inline uint32 GetTypeHash(const FIngredientInstance& Instance)
{
	return HashCombine(GetTypeHash(Instance.IngredientID), GetTypeHash(static_cast<uint8>(Instance.Quality)));
}

// One ingredient occupying one position in one dish at the cooking station.
USTRUCT(BlueprintType)
struct FSlottedIngredient
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName IngredientID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ECookingSlot DishSlot = ECookingSlot::MainDish;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESubSlotType SubSlot = ESubSlotType::Protein;

	// Position within a multi-ingredient sub-slot (e.g. Spice index 0-3).
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 SubSlotIndex = 0;

	// Quality tier — drives GE level for parametric effects.
	// Non-scaling effects (tag grants like GE_Grant_Fire) are unaffected by quality.
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EIngredientQuality Quality = EIngredientQuality::Choice;
};
