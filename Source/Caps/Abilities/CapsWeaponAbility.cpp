#include "Abilities/CapsWeaponAbility.h"
#include "Abilities/CapsAttributeSet.h"
#include "Core/CapsGameSettings.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

void UCapsWeaponAbility::ApplyFoodEffectsOnHit(AActor* HitTarget)
{
	if (!HitTarget) return;

	// Attacker's ASC — we check their active tags against the food effect registry.
	const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo();
	UAbilitySystemComponent* AttackerASC = Info ? Info->AbilitySystemComponent.Get() : nullptr;
	if (!AttackerASC) return;

	// Target's ASC — this is where we apply the conditional hit GEs.
	IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(HitTarget);
	UAbilitySystemComponent* TargetASC = TargetASI ? TargetASI->GetAbilitySystemComponent() : nullptr;
	if (!TargetASC) return;

	const UCapsGameSettings* Settings = UCapsGameSettings::Get();
	for (const FFoodHitEffectEntry& Entry : Settings->FoodHitEffects)
	{
		if (!Entry.HitEffect || !Entry.FoodTag.IsValid()) continue;
		if (!AttackerASC->HasMatchingGameplayTag(Entry.FoodTag)) continue;

		FGameplayEffectContextHandle Context = TargetASC->MakeEffectContext();
		Context.AddInstigator(GetAvatarActorFromActorInfo(), GetAvatarActorFromActorInfo());
		FGameplayEffectSpecHandle Spec = TargetASC->MakeOutgoingSpec(Entry.HitEffect, 1.f, Context);
		TargetASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}

int32 UCapsWeaponAbility::GetProjectileCount() const
{
	const UCapsAttributeSet* Attrs = GetCapsAttributeSet();
	return Attrs ? FMath::Max(1, FMath::FloorToInt(Attrs->GetProjectileCount())) : 1;
}

float UCapsWeaponAbility::GetAttackRadiusScale() const
{
	const UCapsAttributeSet* Attrs = GetCapsAttributeSet();
	return Attrs ? FMath::Max(0.1f, Attrs->GetAttackRadius()) : 1.f;
}
