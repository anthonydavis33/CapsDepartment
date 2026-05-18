#include "UI/CapsIngredientCardWidget.h"
#include "UI/CapsDragDropOperation.h"
#include "Input/Reply.h"

FReply UCapsIngredientCardWidget::NativeOnMouseButtonDown(const FGeometry& InGeometry,
	const FPointerEvent& InMouseEvent)
{
	// Detect left-click only; let right-click propagate for context menus.
	if (InMouseEvent.GetEffectingButton() == EKeys::LeftMouseButton)
	{
		return FReply::Handled().DetectDrag(TakeWidget(), EKeys::LeftMouseButton);
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

void UCapsIngredientCardWidget::NativeOnDragDetected(const FGeometry& /*InGeometry*/,
	const FPointerEvent& /*InMouseEvent*/, UDragDropOperation*& OutOperation)
{
	UCapsDragDropOperation* Op = NewObject<UCapsDragDropOperation>(this);
	Op->IngredientID  = IngredientID;
	Op->SourceSubSlot = OwningSlot;
	Op->DefaultDragVisual = this; // Blueprint can override DragVisualClass instead

	OutOperation = Op;
}
