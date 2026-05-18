#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CapsBossDoorActor.generated.h"

class UBoxComponent;
class UCapsBossModifierDataAsset;
class UCapsBossModifierPoolDataAsset;
class UCapsModifierRevealWidget;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerEnteredBossRoom);

// Placed at the end of the dungeon corridor (or inside the final safe room).
// When the player enters the proximity volume this actor:
//   1. Rolls ModifiersToReveal random entries from ModifierPool (once, stable for the run)
//   2. Shows UCapsModifierRevealWidget with the drawn modifiers
//   3a. Player commits  → applies PlayerEffect GAS effects, fires OnPlayerEnteredBossRoom
//   3b. Player retreats → closes widget, player can leave and adjust build
//
// Blueprint setup guide:
//   - Set ModifierPool and RevealWidgetClass in the Details panel
//   - Bind OnPlayerEnteredBossRoom to trigger the boss level transition
//   - Override BP_OnPlayerEntered for door opening VFX / animation
//   - Override BP_OnPlayerRetreated for door close / reset
UCLASS(Blueprintable)
class CAPS_API ACapsBossDoorActor : public AActor
{
	GENERATED_BODY()

public:
	ACapsBossDoorActor();

	// Bind in Blueprint to trigger the boss level load / transition.
	UPROPERTY(BlueprintAssignable, Category="BossDoor")
	FOnPlayerEnteredBossRoom OnPlayerEnteredBossRoom;

	// The modifiers rolled for this run. Stable after BeginPlay.
	// Read by ACapsBossBase on spawn to activate boss-side GAS effects.
	// Read by room template Blueprints for EnemySpawn / Environmental category behavior.
	UPROPERTY(BlueprintReadOnly, Category="BossDoor")
	TArray<TObjectPtr<UCapsBossModifierDataAsset>> ActiveModifiers;

	// Called by UCapsModifierRevealWidget when the player clicks "Enter".
	UFUNCTION(BlueprintCallable, Category="BossDoor")
	void OnPlayerCommittedEntry();

	// Called by UCapsModifierRevealWidget when the player clicks "Retreat".
	UFUNCTION(BlueprintCallable, Category="BossDoor")
	void OnPlayerRetreated();

protected:
	virtual void BeginPlay() override;

	// Pool to draw from. Assign a UCapsBossModifierPoolDataAsset in the Details panel.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BossDoor")
	TObjectPtr<UCapsBossModifierPoolDataAsset> ModifierPool;

	// How many modifiers to reveal. Default 2.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BossDoor")
	int32 ModifiersToReveal = 2;

	// Widget class to instantiate when the player approaches.
	// Must be a Blueprint child of UCapsModifierRevealWidget.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="BossDoor")
	TSubclassOf<UCapsModifierRevealWidget> RevealWidgetClass;

	// Visual mesh. Assign in Blueprint subclass.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BossDoor")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	// Approach zone. When the player enters this, the modifier reveal widget appears.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="BossDoor")
	TObjectPtr<UBoxComponent> ProximityVolume;

	// Play door opening animation / VFX here.
	UFUNCTION(BlueprintImplementableEvent, Category="BossDoor")
	void BP_OnPlayerEntered();

	// Play door close / reset animation here when player retreats.
	UFUNCTION(BlueprintImplementableEvent, Category="BossDoor")
	void BP_OnPlayerRetreated();

private:
	UPROPERTY()
	TObjectPtr<UCapsModifierRevealWidget> ActiveRevealWidget;

	bool bModifiersRolled = false;

	UFUNCTION()
	void OnProximityBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void RollModifiers();
	void ShowRevealWidget(APlayerController* PC);
	void ApplyPlayerModifierEffects(APawn* Player);
};
