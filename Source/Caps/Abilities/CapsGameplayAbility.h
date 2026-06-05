#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "CapsAbilityInputID.h"
#include "CapsGameplayAbility.generated.h"

class ACapsCharacter;
class UCapsAttributeSet;

// Base class for all player and enemy gameplay abilities.
//
// Activation flow
// ───────────────
//   ActivateAbility (C++) → CommitAbility (cost + cooldown) → BP_ActivateAbility
//   Blueprint calls FinishAbility() when the ability is done.
//   If CommitAbility fails the ability is ended immediately with no effect.
//
// Blueprint implementation guide
// ───────────────────────────────
//   1. Subclass this in Blueprint (e.g. GA_Attack1_Melee).
//   2. Set InputID to match the action you want to trigger it.
//   3. Set CooldownGameplayEffectClass / CostGameplayEffectClass if needed.
//      Create simple GE assets of type Instant (cost) or Duration (cooldown).
//   4. Override BP_ActivateAbility — implement the ability logic here.
//      Use GetCapsCharacter() to reach the pawn, and GetCapsAttributeSet() for stats.
//   5. Call FinishAbility at the end of every code path (including on cancellation).
//      A dangling un-finished ability blocks re-activation.
//
// Instancing: InstancedPerActor by default so Blueprint variables persist across
// activations (e.g. storing a cached hit result between Begin / End).
UCLASS(Abstract, Blueprintable, meta=(ShortTooltip="Base class for all Caps gameplay abilities"))
class CAPS_API UCapsGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UCapsGameplayAbility();

	// Which input action triggers this ability. Must match the player controller's
	// AbilityLocalInputPressed call. None = passive / event-only activation.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability|Input")
	ECapsAbilityInputID InputID = ECapsAbilityInputID::None;

protected:
	// ── Core overrides ─────────────────────────────────────────────────────────

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	// ── Blueprint interface ────────────────────────────────────────────────────

	// Implement the ability here in Blueprint via "Event ActivateAbility".
	// CommitAbility (cost + cooldown) has already succeeded by the time this fires.
	// Call FinishAbility() when done. This is the standard GAS K2_ActivateAbility event.

	// Call this when the ability is fully complete (animation finished, projectile
	// spawned, etc.). Safe to call multiple times — only the first call acts.
	UFUNCTION(BlueprintCallable, Category="Ability")
	void FinishAbility();

	// Call to cancel mid-execution (e.g. interrupted by stagger). Ends the
	// ability and fires EndAbility with bWasCancelled = true.
	// Named AbortAbility to avoid hiding UGameplayAbility::CancelAbility(Handle,...).
	UFUNCTION(BlueprintCallable, Category="Ability")
	void AbortAbility();

	// ── Convenience getters ────────────────────────────────────────────────────

	// Returns the pawn as ACapsCharacter. Null if the owner is not a CapsCharacter
	// (e.g. ability is on an enemy — use GetAvatarActorFromActorInfo directly instead).
	UFUNCTION(BlueprintPure, Category="Ability")
	ACapsCharacter* GetCapsCharacter() const;

	// Returns the owning actor's attribute set. Works for both player and enemy.
	UFUNCTION(BlueprintPure, Category="Ability")
	const UCapsAttributeSet* GetCapsAttributeSet() const;

private:
	// Cached activation context so FinishAbility / CancelAbility can call EndAbility.
	FGameplayAbilitySpecHandle    CachedHandle;
	FGameplayAbilityActivationInfo CachedActivationInfo;
	const FGameplayAbilityActorInfo* CachedActorInfo = nullptr;

	bool bAbilityEnded = false;
};
