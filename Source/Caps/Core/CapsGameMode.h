#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "CapsGameMode.generated.h"

class UCapsCharacterDataAsset;
class ACapsCharacter;
class ACapsDungeonGenerator;

// Shared game mode base for both the hub and dungeon levels.
// Place BP_CapsGameMode (a Blueprint child of this) in each level's World Settings.
//
// Hub level responsibilities:
//   - Show character select widget on BeginPlay.
//   - On player confirmed, store selection in game instance.
//   - Begin dungeon transition when the hub door is activated.
//
// Dungeon level responsibilities:
//   - On BeginPlay: read selected character from game instance, apply to player.
//   - Start the dungeon run (StartRun on the generator).
//   - Handle post-run transitions (death → ReturnToHub, extraction → ReturnToHub).
//
// Blueprint guide:
//   - Override BP_OnTransitionToDungeon for any pre-load cinematic / UI setup.
//   - Override BP_OnTransitionToHub for post-run screen fades.
//   - Bind OnPlayerExtracted / character BP_OnDeath to CallReturnToHub.
UCLASS(Blueprintable)
class CAPS_API ACapsGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ACapsGameMode();

	// ── Transitions ───────────────────────────────────────────────────────────

	// Called by ACapsHubDoorActor when the player commits to entering the dungeon.
	UFUNCTION(BlueprintCallable, Category="GameMode")
	void BeginDungeonTransition();

	// Called after death or successful extraction to return to the hub.
	UFUNCTION(BlueprintCallable, Category="GameMode")
	void ReturnToHub();

	// ── Character application ─────────────────────────────────────────────────

	// Applies the selected character's data (mesh, stat GE, abilities) to the
	// player character. Called automatically on dungeon level BeginPlay.
	UFUNCTION(BlueprintCallable, Category="GameMode")
	void ApplySelectedCharacterToPlayer();

protected:
	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	// Override to play transition VFX / UI before opening the dungeon level.
	UFUNCTION(BlueprintImplementableEvent, Category="GameMode")
	void BP_OnTransitionToDungeon();

	// Override to play screen fade / UI before returning to the hub.
	UFUNCTION(BlueprintImplementableEvent, Category="GameMode")
	void BP_OnTransitionToHub();

	// Override to show character select widget. Called in hub-level BeginPlay.
	UFUNCTION(BlueprintImplementableEvent, Category="GameMode")
	void BP_OnHubReady();

	// Override to run any extra setup after character data is applied in the dungeon.
	UFUNCTION(BlueprintImplementableEvent, Category="GameMode")
	void BP_OnDungeonReady();

private:
	bool bIsDungeonLevel() const;
	void StartDungeonRun();
	void ApplyCharacterData(ACapsCharacter* Character, UCapsCharacterDataAsset* CharData);
};
