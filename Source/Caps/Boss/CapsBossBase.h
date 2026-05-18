#pragma once

#include "CoreMinimal.h"
#include "Characters/CapsEnemyBase.h"
#include "GameplayEffectTypes.h"
#include "CapsBossBase.generated.h"

class UCapsBossModifierDataAsset;

// C++ base for all bosses. Subclass in Blueprint to set mesh, animations, and
// implement attack abilities per phase.
//
// Phase system
// ─────────────
//   CurrentPhase starts at 0. When Health drops to or below
//   Phase2HealthThreshold × MaxHealth the phase increments to 1 and
//   BP_OnPhaseChanged(1) fires. Extend with additional thresholds in Blueprint.
//
// Modifier system
// ───────────────
//   On BeginPlay the boss auto-detects ACapsBossDoorActor in the world,
//   reads its ActiveModifiers, and calls ActivateModifiers() which applies
//   each modifier's BossEffect GAS gameplay effect.
//   Room template Blueprints can also read ActiveModifiers directly for
//   EnemySpawn / Environmental category modifiers.
UCLASS(Abstract, Blueprintable)
class CAPS_API ACapsBossBase : public ACapsEnemyBase
{
	GENERATED_BODY()

public:
	ACapsBossBase();

	// Modifiers active for this run. Populated in BeginPlay from the boss door.
	// Read by room template Blueprints for spawn / environmental modifier behavior.
	UPROPERTY(BlueprintReadOnly, Category="Boss")
	TArray<TObjectPtr<UCapsBossModifierDataAsset>> ActiveModifiers;

	// Applies modifier BossEffects via GAS and populates ActiveModifiers.
	// Called automatically in BeginPlay; can also be called manually if needed.
	UFUNCTION(BlueprintCallable, Category="Boss")
	void ActivateModifiers(const TArray<UCapsBossModifierDataAsset*>& Modifiers);

	// Current phase (0 = initial, 1 = triggered by Phase2HealthThreshold).
	UPROPERTY(BlueprintReadOnly, Category="Boss")
	int32 CurrentPhase = 0;

	// Health fraction (0–1) at which phase 2 triggers.
	// Default 0.5 = triggers when the boss hits 50% HP.
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Boss")
	float Phase2HealthThreshold = 0.5f;

protected:
	virtual void BeginPlay() override;

	// Override in Blueprint to swap attack patterns, play cinematics, etc.
	UFUNCTION(BlueprintImplementableEvent, Category="Boss")
	void BP_OnPhaseChanged(int32 NewPhase);

private:
	bool bPhase2Triggered = false;

	// Second health-change listener (base class handles death; this handles phases).
	void OnBossHealthChanged(const FOnAttributeChangeData& Data);
};
