#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "Cooking/CookingTypes.h"
#include "CapsInventoryComponent.generated.h"

class UIngredientDataAsset;
class UReactionDataAsset;
class UAbilitySystemComponent;

UCLASS(ClassGroup=Caps, meta=(BlueprintSpawnableComponent))
class CAPS_API UCapsInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCapsInventoryComponent();

	// All known reactions — assign in BP_CapsCharacter's Details panel.
	// Checked per-dish when EatMeal() is called.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Inventory|Reactions")
	TArray<TObjectPtr<UReactionDataAsset>> KnownReactions;

	// ── Base Stock ─────────────────────────────────────────────────────────────
	// Hub inventory. Ingredients consumed when slotted, returned when unslotted.

	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	TMap<FIngredientInstance, int32> BaseStock;

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void AddToBaseStock(FName IngredientID, EIngredientQuality Quality = EIngredientQuality::Choice, int32 Quantity = 1);

	// Returns false if not enough stock.
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool RemoveFromBaseStock(FName IngredientID, EIngredientQuality Quality, int32 Quantity = 1);

	// Returns count for a specific quality tier.
	UFUNCTION(BlueprintPure, Category="Inventory")
	int32 GetBaseStockQuantity(FName IngredientID, EIngredientQuality Quality) const;

	// Returns total count across all quality tiers (useful for UI display).
	UFUNCTION(BlueprintPure, Category="Inventory")
	int32 GetTotalBaseStockQuantity(FName IngredientID) const;

	// ── Cooking Station ────────────────────────────────────────────────────────
	// Current sub-slot configuration across all dishes.

	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	TArray<FSlottedIngredient> SlottedIngredients;

	// Moves one unit from BaseStock into the given sub-slot.
	// Returns false if out of stock or the sub-slot is already occupied.
	UFUNCTION(BlueprintCallable, Category="Inventory")
	bool SlotIngredient(FName IngredientID, EIngredientQuality Quality, ECookingSlot Dish, ESubSlotType SubSlot, int32 SubSlotIndex = 0);

	// Returns the ingredient at that position to BaseStock.
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void UnslotIngredient(ECookingSlot Dish, ESubSlotType SubSlot, int32 SubSlotIndex = 0);

	// Returns all ingredients in a dish to BaseStock.
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void ClearDish(ECookingSlot Dish);

	UFUNCTION(BlueprintPure, Category="Inventory")
	TArray<FSlottedIngredient> GetDishIngredients(ECookingSlot Dish) const;

	// ── Meal & Run ─────────────────────────────────────────────────────────────

	// Snapshot of what was slotted when the meal was eaten. Drives GAS effects.
	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	TArray<FSlottedIngredient> ActiveMeal;

	// Ingredients found during the current run — not yet in BaseStock.
	UPROPERTY(BlueprintReadOnly, Category="Inventory")
	TMap<FIngredientInstance, int32> RunPickups;

	// Locks in the current cooking station state as the active meal,
	// applies GAS ingredient effects per dish, then resolves reactions.
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void EatMeal();

	UFUNCTION(BlueprintCallable, Category="Inventory")
	void AddRunPickup(FName IngredientID, EIngredientQuality Quality = EIngredientQuality::Choice, int32 Quantity = 1);

	// ── Extraction / Death ─────────────────────────────────────────────────────

	// All RunPickups merge into BaseStock. Called on successful extraction.
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void HandleExtraction();

	// 50% of combined (BaseStock + RunPickups) is lost at random.
	// Survivors return to BaseStock.
	UFUNCTION(BlueprintCallable, Category="Inventory")
	void HandleDeath();

private:
	UIngredientDataAsset* LoadIngredient(FName IngredientID) const;
	void ApplyDishEffects(ECookingSlot Dish, UAbilitySystemComponent* ASC);
	void ResolveReactions(ECookingSlot Dish, const FGameplayTagContainer& DishTags, UAbilitySystemComponent* ASC);

	static const ECookingSlot AllSlots[];
};
