#include "Abilities/CapsGameplayAbility.h"
#include "Abilities/CapsAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CapsCharacter.h"

UCapsGameplayAbility::UCapsGameplayAbility()
{
	// InstancedPerActor: one instance lives on the ASC and is reused each activation.
	// Blueprint variables (e.g. cached refs, timers) persist between activations.
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	// Abilities are blocked while the owner is dead.
	ActivationBlockedTags.AddTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
}

void UCapsGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	UE_LOG(LogTemp, Warning, TEXT("ActivateAbility: %s"), *GetName());

	// Cache context so FinishAbility / CancelAbility can call EndAbility.
	CachedHandle         = Handle;
	CachedActorInfo      = ActorInfo;
	CachedActivationInfo = ActivationInfo;
	bAbilityEnded        = false;

	// CommitAbility applies cost and cooldown GEs. If either check fails
	// (e.g. not enough resource, still on cooldown) end immediately.
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		UE_LOG(LogTemp, Error, TEXT("ActivateAbility: CommitAbility FAILED for %s"), *GetName());
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	K2_ActivateAbility();
}

void UCapsGameplayAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	bAbilityEnded = true;
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UCapsGameplayAbility::FinishAbility()
{
	if (bAbilityEnded || !CachedActorInfo) return;
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo,
		/*bReplicateEndAbility=*/true, /*bWasCancelled=*/false);
}

void UCapsGameplayAbility::AbortAbility()
{
	if (bAbilityEnded || !CachedActorInfo) return;
	EndAbility(CachedHandle, CachedActorInfo, CachedActivationInfo,
		/*bReplicateEndAbility=*/true, /*bWasCancelled=*/true);
}

ACapsCharacter* UCapsGameplayAbility::GetCapsCharacter() const
{
	return Cast<ACapsCharacter>(GetAvatarActorFromActorInfo());
}

const UCapsAttributeSet* UCapsGameplayAbility::GetCapsAttributeSet() const
{
	const FGameplayAbilityActorInfo* Info = GetCurrentActorInfo();
	if (!Info || !Info->AbilitySystemComponent.IsValid()) return nullptr;

	return Info->AbilitySystemComponent->GetSet<UCapsAttributeSet>();
}
