#include "Core/CapsGameInstance.h"
#include "Core/CapsRunSaveGame.h"
#include "Items/CapsInventoryComponent.h"
#include "Kismet/GameplayStatics.h"

UCapsGameInstance* UCapsGameInstance::Get(const UObject* WorldContextObject)
{
	return Cast<UCapsGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

void UCapsGameInstance::Init()
{
	Super::Init();
	LoadSaveData();
}

void UCapsGameInstance::LoadSaveData()
{
	if (UGameplayStatics::DoesSaveGameExist(UCapsRunSaveGame::SaveSlotName, UCapsRunSaveGame::SaveUserIndex))
	{
		ActiveSave = Cast<UCapsRunSaveGame>(
			UGameplayStatics::LoadGameFromSlot(UCapsRunSaveGame::SaveSlotName, UCapsRunSaveGame::SaveUserIndex));
	}

	if (!ActiveSave)
	{
		// First launch or corrupt save — start clean.
		ActiveSave = Cast<UCapsRunSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UCapsRunSaveGame::StaticClass()));
	}

	// Mirror the progression flag so the rest of the system can read it off the GI.
	if (ActiveSave)
		bFirstRunComplete = ActiveSave->bFirstRunComplete;
}

void UCapsGameInstance::SelectCharacter(UCapsCharacterDataAsset* CharacterData)
{
	SelectedCharacter = CharacterData;
}

void UCapsGameInstance::SetRunState(ECapsRunState NewState)
{
	RunState = NewState;
}

void UCapsGameInstance::NotifyRunEnded()
{
	bFirstRunComplete = true;
	RunState = ECapsRunState::Hub;
}

void UCapsGameInstance::SaveCurrentRun(UCapsInventoryComponent* Inventory)
{
	if (!ActiveSave)
	{
		ActiveSave = Cast<UCapsRunSaveGame>(
			UGameplayStatics::CreateSaveGameObject(UCapsRunSaveGame::StaticClass()));
	}
	if (!ActiveSave) return;

	// Snapshot the player's hub inventory.
	if (Inventory)
		ActiveSave->BaseStock = Inventory->BaseStock;

	ActiveSave->bFirstRunComplete = bFirstRunComplete;

	UGameplayStatics::SaveGameToSlot(ActiveSave, UCapsRunSaveGame::SaveSlotName, UCapsRunSaveGame::SaveUserIndex);
}

void UCapsGameInstance::RestoreInventoryFromSave(UCapsInventoryComponent* Inventory) const
{
	if (!Inventory || !ActiveSave) return;

	// Populate hub stock from the saved snapshot.
	// AddToBaseStock merges quantities, so repeated calls won't double-count
	// as long as the inventory starts empty (which it does on a fresh pawn).
	for (const auto& Pair : ActiveSave->BaseStock)
	{
		if (!Pair.Key.IsNone() && Pair.Value > 0)
			Inventory->AddToBaseStock(Pair.Key, Pair.Value);
	}
}
