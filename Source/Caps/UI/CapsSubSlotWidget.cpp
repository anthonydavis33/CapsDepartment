#include "UI/CapsSubSlotWidget.h"
#include "UI/CapsDragDropOperation.h"
#include "Items/CapsInventoryComponent.h"
#include "CapsCharacter.h"

void UCapsSubSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Resolve inventory once at construct — the widget lives on the player's HUD.
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ACapsCharacter* Char = Cast<ACapsCharacter>(PC->GetPawn()))
			CachedInventory = Char->GetInventoryComponent();
	}

	// Sync display with any pre-existing slot state.
	if (CachedInventory)
	{
		const FSlottedIngredient* Found = CachedInventory->GetDishIngredients(DishSlot).FindByPredicate(
			[this](const FSlottedIngredient& S)
			{
				return S.SubSlot == SubSlot && S.SubSlotIndex == SubSlotIndex;
			});
		CurrentIngredientID = Found ? Found->IngredientID : NAME_None;
	}

	OnSlotContentChanged(CurrentIngredientID);
}

FReply UCapsSubSlotWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	// Only start a drag if there's an ingredient to drag out.
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton
		&& CurrentIngredientID != NAME_None)
	{
		return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UCapsSubSlotWidget::NativeOnDragDetected(const FGeometry& /*InGeometry*/,
	const FPointerEvent& /*InMouseEvent*/, UDragDropOperation*& OutOperation)
{
	if (CurrentIngredientID == NAME_None) return;

	UCapsDragDropOperation* Op = NewObject<UCapsDragDropOperation>(this);
	Op->IngredientID  = CurrentIngredientID;
	Op->SourceSubSlot = this;
	Op->DefaultDragVisual = this;

	// Optimistically clear the slot display while dragging.
	// Restored in NativeOnDragLeave if the drag is cancelled.
	CurrentIngredientID = NAME_None;
	OnSlotContentChanged(NAME_None);

	OutOperation = Op;
}

bool UCapsSubSlotWidget::NativeOnDrop(const FGeometry& /*InGeometry*/,
	const FDragDropEvent& /*InDragDropEvent*/, UDragDropOperation* InOperation)
{
	const UCapsDragDropOperation* Op = Cast<UCapsDragDropOperation>(InOperation);
	if (!Op || Op->IngredientID == NAME_None || !CachedInventory) return false;

	// If the drag came from another slot, unslot it first so the ingredient
	// goes back to BaseStock before we re-slot it here.
	if (Op->SourceSubSlot && Op->SourceSubSlot != this)
		Op->SourceSubSlot->ClearSlot();

	SetSlotIngredient(Op->IngredientID);
	return true;
}

void UCapsSubSlotWidget::NativeOnDragEnter(const FGeometry& /*InGeometry*/,
	const FDragDropEvent& /*InDragDropEvent*/, UDragDropOperation* InOperation)
{
	if (Cast<UCapsDragDropOperation>(InOperation))
		OnDragHoverBegin();
}

void UCapsSubSlotWidget::NativeOnDragLeave(const FDragDropEvent& /*InDragDropEvent*/,
	UDragDropOperation* InOperation)
{
	if (Cast<UCapsDragDropOperation>(InOperation))
		OnDragHoverEnd();
}

void UCapsSubSlotWidget::SetSlotIngredient(FName IngredientID)
{
	if (!CachedInventory) return;

	// Slot returns false when the ingredient isn't in BaseStock — don't update display.
	if (!CachedInventory->SlotIngredient(IngredientID, DishSlot, SubSlot, SubSlotIndex))
		return;

	CurrentIngredientID = IngredientID;
	OnSlotContentChanged(CurrentIngredientID);
}

void UCapsSubSlotWidget::ClearSlot()
{
	if (!CachedInventory || CurrentIngredientID == NAME_None) return;

	CachedInventory->UnslotIngredient(DishSlot, SubSlot, SubSlotIndex);
	CurrentIngredientID = NAME_None;
	OnSlotContentChanged(NAME_None);
}
