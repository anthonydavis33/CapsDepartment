#pragma once

#include "AbilitySystemComponent.h"
#include "CapsAbilityInputID.h"
#include "CapsAbilitySystemComponent.generated.h"

class UCapsGameplayAbility;

// Thin subclass of UAbilitySystemComponent.
// Adds a typed GrantAbility helper so callers don't have to build
// FGameplayAbilitySpec manually, and exposes typed input forwarding.
UCLASS()
class CAPS_API UCapsAbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()

public:
	// Grants an ability and binds it to the given input slot.
	// Returns the spec handle — store it if you need to remove the ability later.
	FGameplayAbilitySpecHandle GrantAbility(TSubclassOf<UCapsGameplayAbility> AbilityClass,
		int32 Level = 1);

	// Forward a press event to GAS. Called by the player controller on each action.
	void PressInputID(ECapsAbilityInputID InputID);

	// Forward a release event (for held / toggle abilities).
	void ReleaseInputID(ECapsAbilityInputID InputID);
};
