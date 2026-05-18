#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CapsCharacterSelectWidget.generated.h"

class UCapsCharacterDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSelectionConfirmed,
	UCapsCharacterDataAsset*, SelectedCharacter);

// C++ base for the character select screen shown at the start of each run.
//
// NativeConstruct reads UCapsGameSettings::AvailableCharacters and calls
// OnCharactersLoaded so Blueprint can build the character card layout.
//
// Blueprint implementation guide:
//   - Override OnCharactersLoaded to create one card widget per entry.
//   - Call PreviewCharacter(Data) when the player hovers a card to populate
//     the details panel (name, portrait, pros/cons).
//   - Call ConfirmSelection() when the player clicks "Start Run".
//     ConfirmSelection stores the choice in UCapsGameInstance and fires
//     OnSelectionConfirmed — bind this to GameMode->BeginDungeonTransition().
UCLASS(Abstract, Blueprintable)
class CAPS_API UCapsCharacterSelectWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Fires when the player confirms their selection. Bind in Blueprint
	// (or GameMode) to trigger the dungeon transition.
	UPROPERTY(BlueprintAssignable, Category="CharacterSelect")
	FOnCharacterSelectionConfirmed OnSelectionConfirmed;

	// Highlight a character card on hover. Updates the preview panel.
	UFUNCTION(BlueprintCallable, Category="CharacterSelect")
	void PreviewCharacter(UCapsCharacterDataAsset* CharacterData);

	// Lock in the previewed character and fire OnSelectionConfirmed.
	// Does nothing if no character has been previewed yet.
	UFUNCTION(BlueprintCallable, Category="CharacterSelect")
	void ConfirmSelection();

protected:
	virtual void NativeConstruct() override;

	// Called once with all available characters from UCapsGameSettings.
	// Build your card grid from this list in Blueprint.
	UFUNCTION(BlueprintImplementableEvent, Category="CharacterSelect")
	void OnCharactersLoaded(const TArray<UCapsCharacterDataAsset*>& Characters);

	// Called on hover — populate name, portrait, pros/cons panel from Data.
	UFUNCTION(BlueprintImplementableEvent, Category="CharacterSelect")
	void OnCharacterPreviewed(UCapsCharacterDataAsset* Data);

private:
	UPROPERTY()
	TObjectPtr<UCapsCharacterDataAsset> PreviewedCharacter;
};
