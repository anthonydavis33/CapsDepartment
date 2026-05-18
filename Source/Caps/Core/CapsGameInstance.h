#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "CapsGameInstance.generated.h"

class UCapsCharacterDataAsset;

UENUM(BlueprintType)
enum class ECapsRunState : uint8
{
	Hub           UMETA(DisplayName="Hub"),           // Player is in the hub between runs.
	Transitioning UMETA(DisplayName="Transitioning"), // Level load in progress.
	InRun         UMETA(DisplayName="In Run"),        // Active dungeon run.
	BossRoom      UMETA(DisplayName="Boss Room"),     // Player committed to the boss fight.
};

// Persists across level loads. Stores run-to-run state: selected character,
// run state, and progression flags like boss unlock.
//
// Access from anywhere: UCapsGameInstance::Get(WorldContextObject)
UCLASS()
class CAPS_API UCapsGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	// ── Static accessor ───────────────────────────────────────────────────────

	static UCapsGameInstance* Get(const UObject* WorldContextObject);

	// ── Character selection ───────────────────────────────────────────────────

	// The character chosen on the select screen. Null until first selection.
	UPROPERTY(BlueprintReadOnly, Category="Character")
	TObjectPtr<UCapsCharacterDataAsset> SelectedCharacter;

	UFUNCTION(BlueprintCallable, Category="Character")
	void SelectCharacter(UCapsCharacterDataAsset* CharacterData);

	// ── Run state ─────────────────────────────────────────────────────────────

	UPROPERTY(BlueprintReadOnly, Category="Run")
	ECapsRunState RunState = ECapsRunState::Hub;

	UFUNCTION(BlueprintCallable, Category="Run")
	void SetRunState(ECapsRunState NewState);

	// ── Progression ───────────────────────────────────────────────────────────

	// True once the player has completed (or failed) their first run.
	// The boss door in the hub only appears after this is set.
	UPROPERTY(BlueprintReadOnly, Category="Progression")
	bool bFirstRunComplete = false;

	// Call after a run ends (extraction or death) to update progression flags.
	UFUNCTION(BlueprintCallable, Category="Progression")
	void NotifyRunEnded();
};
