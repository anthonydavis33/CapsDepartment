#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Cooking/CookingTypes.h"
#include "CapsSubSlotWidget.generated.h"

class UCapsInventoryComponent;

// C++ base for a single cooking sub-slot (e.g. the Protein slot of Main Dish).
// Acts as both a drag SOURCE (drag the current ingredient out) and a drop TARGET.
//
// Blueprint implementation guide:
//   - Set DishSlot, SubSlot, and SubSlotIndex in the Details panel for each slot widget.
//   - Override OnSlotContentChanged to show / hide the ingredient card.
//   - Override OnDragHoverBegin / OnDragHoverEnd to highlight / dim the drop zone.
//   - InventoryComponent is auto-resolved in NativeConstruct from the owning player.
UCLASS(Abstract, Blueprintable)
class CAPS_API UCapsSubSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Which dish this sub-slot belongs to. Set in the Details panel.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SubSlot")
	ECookingSlot DishSlot = ECookingSlot::MainDish;

	// Which sub-slot type within that dish. Set in the Details panel.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SubSlot")
	ESubSlotType SubSlot = ESubSlotType::Protein;

	// Index for multi-ingredient sub-slots (e.g. Spice 0-3). Usually 0.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="SubSlot")
	int32 SubSlotIndex = 0;

	// Current ingredient in this slot. NAME_None = empty.
	UPROPERTY(BlueprintReadOnly, Category="SubSlot")
	FName CurrentIngredientID;

	// Quality of the current ingredient. Read this in OnSlotContentChanged to
	// show the quality badge (Table / Choice / Prime) on the slot display.
	UPROPERTY(BlueprintReadOnly, Category="SubSlot")
	EIngredientQuality CurrentQuality = EIngredientQuality::Choice;

	// Remove the ingredient, return it to BaseStock, and refresh the visual.
	UFUNCTION(BlueprintCallable, Category="SubSlot")
	void ClearSlot();

protected:
	virtual void NativeConstruct() override;

	// Drag-from-slot: left-click an occupied slot starts a drag of the current ingredient.
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnDragDetected(const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	// Drop target: accept a UCapsDragDropOperation.
	virtual bool NativeOnDrop(const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;

	virtual void NativeOnDragEnter(const FGeometry& InGeometry,
		const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	virtual void NativeOnDragLeave(const FDragDropEvent& InDragDropEvent,
		UDragDropOperation* InOperation) override;

	// Refresh the displayed ingredient card. Called after any slot change.
	UFUNCTION(BlueprintImplementableEvent, Category="SubSlot")
	void OnSlotContentChanged(FName NewIngredientID);

	// Highlight the slot while a compatible drag hovers over it.
	UFUNCTION(BlueprintImplementableEvent, Category="SubSlot")
	void OnDragHoverBegin();

	UFUNCTION(BlueprintImplementableEvent, Category="SubSlot")
	void OnDragHoverEnd();

private:
	UPROPERTY()
	TObjectPtr<UCapsInventoryComponent> CachedInventory;

	void SetSlotIngredient(FName IngredientID, EIngredientQuality Quality);
};
