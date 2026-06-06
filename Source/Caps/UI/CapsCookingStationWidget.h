#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Cooking/CookingTypes.h"
#include "CapsCookingStationWidget.generated.h"

class UCapsInventoryComponent;

// C++ base for the hub cooking station UI.
// Blueprint child builds the dish panels with UCapsSubSlotWidgets and
// binds the "Eat Meal" button to OnEatMealPressed().
//
// Blueprint implementation guide:
//   - In the designer, create UCapsSubSlotWidget children for each slot.
//     Set DishSlot / SubSlot / SubSlotIndex directly in the Details panel per widget.
//     The sub-slot widgets self-wire to the inventory via the owning player pawn.
//   - OnStationReady fires in NativeConstruct once the inventory is found.
//     Use it to call RefreshInventoryDisplay() and populate ingredient cards.
//   - Bind your "Eat Meal" button's OnClicked → OnEatMealPressed.
//   - Override BP_OnMealEaten to play the eating animation / close the UI.
//   - Override BP_OnEatFailed if you want feedback when EatMeal() is called
//     with an empty loadout (nothing slotted).
UCLASS(Abstract, Blueprintable)
class CAPS_API UCapsCookingStationWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Eats the current meal. Wire to your "Eat Meal" button's OnClicked.
	UFUNCTION(BlueprintCallable, Category="CookingStation")
	void OnEatMealPressed();

	// Refreshes the inventory display. Call after dropping ingredients
	// or returning from a run to reflect updated BaseStock quantities.
	UFUNCTION(BlueprintCallable, Category="CookingStation")
	void RefreshInventoryDisplay();

protected:
	virtual void NativeConstruct() override;

	// Fired once NativeConstruct resolves the inventory component.
	// Build the initial ingredient grid here.
	UFUNCTION(BlueprintImplementableEvent, Category="CookingStation")
	void OnStationReady(UCapsInventoryComponent* Inventory);

	// Called after EatMeal() succeeds — play eat animation, close UI, etc.
	UFUNCTION(BlueprintImplementableEvent, Category="CookingStation")
	void BP_OnMealEaten();

	// Called if OnEatMealPressed is triggered with no ingredients slotted.
	UFUNCTION(BlueprintImplementableEvent, Category="CookingStation")
	void BP_OnEatFailed();

	// Called by RefreshInventoryDisplay — rebuild ingredient card widgets from Stock.
	// Stock maps FIngredientInstance (IngredientID + Quality) → quantity.
	// In Blueprint, iterate the map to build one card per entry.
	// Use GetDisplayName on the Quality enum + Ingredient->DisplayName to label each card.
	UFUNCTION(BlueprintImplementableEvent, Category="CookingStation")
	void OnInventoryRefreshed(const TMap<FIngredientInstance, int32>& Stock);

private:
	UPROPERTY()
	TObjectPtr<UCapsInventoryComponent> CachedInventory;
};
