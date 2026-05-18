#include "Abilities/CapsAbilitySystemComponent.h"
#include "Abilities/CapsGameplayAbility.h"

FGameplayAbilitySpecHandle UCapsAbilitySystemComponent::GrantAbility(
	TSubclassOf<UCapsGameplayAbility> AbilityClass, int32 Level)
{
	if (!AbilityClass) return FGameplayAbilitySpecHandle();

	// Read InputID from the CDO so callers don't need to supply it manually.
	const UCapsGameplayAbility* CDO = AbilityClass->GetDefaultObject<UCapsGameplayAbility>();
	const int32 InputID = static_cast<int32>(CDO->InputID);

	FGameplayAbilitySpec Spec(AbilityClass, Level, InputID);
	return GiveAbility(Spec);
}

void UCapsAbilitySystemComponent::PressInputID(ECapsAbilityInputID InputID)
{
	AbilityLocalInputPressed(static_cast<int32>(InputID));
}

void UCapsAbilitySystemComponent::ReleaseInputID(ECapsAbilityInputID InputID)
{
	AbilityLocalInputReleased(static_cast<int32>(InputID));
}
