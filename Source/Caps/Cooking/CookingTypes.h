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
