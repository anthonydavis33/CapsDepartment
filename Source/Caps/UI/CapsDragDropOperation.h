#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "Cooking/CookingTypes.h"
#include "CapsDragDropOperation.generated.h"

class UCapsSubSlotWidget;

// Payload carried during all cooking-station drag operations.
// Created by UCapsIngredientCardWidget and UCapsSubSlotWidget when dragging begins.
//
// SourceSubSlot: set when dragging OUT of an occupied slot.
//   Null when dragging from the inventory grid — slot will not be unslotted on drop.
UCLASS()
class CAPS_API UCapsDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()

public:
	// The ingredient being dragged.
	UPROPERTY(BlueprintReadWrite, Category="DragDrop")
	FName IngredientID;

	// Quality tier of the ingredient being dragged.
	UPROPERTY(BlueprintReadWrite, Category="DragDrop")
	EIngredientQuality Quality = EIngredientQuality::Choice;

	// The sub-slot this drag originated from. Null if dragging from the inventory grid.
	// On a successful drop the destination slot will call UnslotIngredient on this.
	UPROPERTY(BlueprintReadWrite, Category="DragDrop")
	TObjectPtr<UCapsSubSlotWidget> SourceSubSlot;
};
