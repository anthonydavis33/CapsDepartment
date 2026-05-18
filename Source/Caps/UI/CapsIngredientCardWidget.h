#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CapsIngredientCardWidget.generated.h"

class UCapsSubSlotWidget;

// C++ base for a draggable ingredient card.
// Lives either in UCapsInventoryGridWidget (inventory source) or inside a
// UCapsSubSlotWidget (slot source).
//
// Blueprint implementation guide:
//   - Override OnIngredientSet to display the ingredient's icon and name.
//     Load the asset via UIngredientDataAsset::LoadSynchronous() or AsyncLoad.
//   - The thumbnail widget class set on DragVisualClass will be shown under
//     the cursor while dragging — make it a lightweight version of this card.
UCLASS(Abstract, Blueprintable)
class CAPS_API UCapsIngredientCardWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Set this before adding the widget to the viewport.
	UPROPERTY(BlueprintReadWrite, Category="Card")
	FName IngredientID;

	// Set when this card lives inside a sub-slot (drag-from-slot path).
	// Null when this card is in the inventory grid (drag-from-inventory path).
	UPROPERTY(BlueprintReadWrite, Category="Card")
	TObjectPtr<UCapsSubSlotWidget> OwningSlot;

protected:
	// NativeOnMouseButtonDown begins drag detection on left click.
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent) override;

	// NativeOnDragDetected creates the drag operation.
	virtual void NativeOnDragDetected(const FGeometry& InGeometry,
		const FPointerEvent& InMouseEvent, UDragDropOperation*& OutOperation) override;

	// Called after IngredientID is set — rebuild visuals (icon, name, tags).
	UFUNCTION(BlueprintImplementableEvent, Category="Card")
	void OnIngredientSet(FName NewIngredientID);
};
