#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "CapsModifierRevealWidget.generated.h"

class UCapsBossModifierDataAsset;
class ACapsBossDoorActor;

// C++ base for the boss modifier reveal widget.
// Shown when the player enters the boss door proximity volume.
//
// Blueprint implementation guide:
//   - Override OnModifiersReady to build modifier cards from the Entries array.
//     Each entry exposes DisplayName, Description, Category, and Icon.
//   - Wire "Enter" button → CommitToEntry()
//   - Wire "Retreat" button → Retreat()
//   - Both buttons auto-restore game input and remove the widget.
//   - Category can be used to color-code cards (PlayerDebuff = red, BossEnhancement = purple, etc.)
UCLASS(Abstract, Blueprintable)
class CAPS_API UCapsModifierRevealWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Called by ACapsBossDoorActor immediately after creation. Do not call manually.
	void InitializeModifiers(const TArray<UCapsBossModifierDataAsset*>& Modifiers, ACapsBossDoorActor* Door);

	// Wire to the "Enter Boss Room" button.
	UFUNCTION(BlueprintCallable, Category="BossReveal")
	void CommitToEntry();

	// Wire to the "Retreat" / "Go Back" button.
	UFUNCTION(BlueprintCallable, Category="BossReveal")
	void Retreat();

protected:
	// Called once after InitializeModifiers populates the data.
	// Build your modifier cards from the Modifiers array here.
	UFUNCTION(BlueprintImplementableEvent, Category="BossReveal")
	void OnModifiersReady(const TArray<UCapsBossModifierDataAsset*>& Modifiers);

private:
	UPROPERTY()
	TObjectPtr<ACapsBossDoorActor> OwningDoor;
};
