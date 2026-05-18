#include "UI/CapsCookingStationWidget.h"
#include "Items/CapsInventoryComponent.h"
#include "CapsCharacter.h"

void UCapsCookingStationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (ACapsCharacter* Char = Cast<ACapsCharacter>(PC->GetPawn()))
			CachedInventory = Char->GetInventoryComponent();
	}

	if (CachedInventory)
	{
		OnStationReady(CachedInventory);
		RefreshInventoryDisplay();
	}
}

void UCapsCookingStationWidget::OnEatMealPressed()
{
	if (!CachedInventory) return;

	// Treat an entirely empty loadout as a no-op so the player gets feedback
	// without silently eating a blank meal.
	if (CachedInventory->SlottedIngredients.IsEmpty())
	{
		BP_OnEatFailed();
		return;
	}

	CachedInventory->EatMeal();
	BP_OnMealEaten();
}

void UCapsCookingStationWidget::RefreshInventoryDisplay()
{
	if (!CachedInventory) return;
	OnInventoryRefreshed(CachedInventory->BaseStock);
}
