#pragma once

#include "CoreMinimal.h"
#include "Abilities/CapsGameplayAbility.h"
#include "CapsWeaponAbility.generated.h"

// Base class for ALL weapon abilities — both main and secondary.
// Provides food-effect application and attribute helpers so every weapon
// automatically benefits from the character's current meal modifiers.
//
// Blueprint implementation guide:
//   - Set InputID = Attack1 for main weapons, Attack2 for secondaries.
//   - Override BP_ActivateAbility → run your attack logic (spawn projectile,
//     trace, play montage, etc.).
//   - When a hit lands, call ApplyFoodEffectsOnHit(HitActor) — it reads
//     UCapsGameSettings::FoodHitEffects and applies matching GAS effects
//     to the target based on the character's active food tags.
//   - Call FinishAbility() when the attack sequence is complete.
//
// Food modifier helpers (read-only, no parameters):
//   GetProjectileCount()  — how many projectiles to spawn this activation
//   GetAttackRadiusScale() — multiplier for hit sphere / projectile scale
UCLASS(Abstract, Blueprintable)
class CAPS_API UCapsWeaponAbility : public UCapsGameplayAbility
{
	GENERATED_BODY()

public:
	// Applies all active food effects (from UCapsGameSettings::FoodHitEffects)
	// to the target. Call this whenever a hit is confirmed.
	// Works for any actor with an AbilitySystemComponent.
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void ApplyFoodEffectsOnHit(AActor* HitTarget);

	// Returns the current ProjectileCount attribute value (floored to int, min 1).
	// Use this to decide how many projectiles / rays to spawn per activation.
	UFUNCTION(BlueprintPure, Category="Weapon")
	int32 GetProjectileCount() const;

	// Returns the current AttackRadius attribute value.
	// Multiply your base hit-sphere radius or projectile scale by this.
	UFUNCTION(BlueprintPure, Category="Weapon")
	float GetAttackRadiusScale() const;
};
