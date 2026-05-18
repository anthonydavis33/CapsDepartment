#include "UI/CapsCharacterSelectWidget.h"
#include "Core/CapsGameSettings.h"
#include "Core/CapsGameInstance.h"
#include "Characters/CapsCharacterDataAsset.h"

void UCapsCharacterSelectWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// Load all available character assets and hand them to Blueprint.
	TArray<UCapsCharacterDataAsset*> LoadedCharacters;
	for (const TSoftObjectPtr<UCapsCharacterDataAsset>& SoftRef :
		UCapsGameSettings::Get()->AvailableCharacters)
	{
		if (UCapsCharacterDataAsset* Data = SoftRef.LoadSynchronous())
			LoadedCharacters.Add(Data);
	}

	OnCharactersLoaded(LoadedCharacters);

	// Re-select the last character if one was already chosen this session.
	if (UCapsGameInstance* GI = UCapsGameInstance::Get(this))
	{
		if (GI->SelectedCharacter)
			PreviewCharacter(GI->SelectedCharacter);
	}
}

void UCapsCharacterSelectWidget::PreviewCharacter(UCapsCharacterDataAsset* CharacterData)
{
	if (!CharacterData) return;
	PreviewedCharacter = CharacterData;
	OnCharacterPreviewed(CharacterData);
}

void UCapsCharacterSelectWidget::ConfirmSelection()
{
	if (!PreviewedCharacter) return;

	if (UCapsGameInstance* GI = UCapsGameInstance::Get(this))
		GI->SelectCharacter(PreviewedCharacter);

	OnSelectionConfirmed.Broadcast(PreviewedCharacter);
}
