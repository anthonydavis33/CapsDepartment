#pragma once

#include "CoreMinimal.h"
#include "CapsAbilityInputID.generated.h"

// Maps each player input action to a stable integer ID used by GAS to route
// input presses to the correct ability spec.
//
// When granting an ability, set FGameplayAbilitySpec::InputID to the int32 cast
// of the matching value here. The player controller calls
// AbilityLocalInputPressed(int32(ECapsAbilityInputID::X)) on each key press.
//
// None (0) is reserved — abilities with InputID == None are never input-activated
// (use them for passive or event-triggered abilities instead).
UENUM(BlueprintType)
enum class ECapsAbilityInputID : uint8
{
	None        = 0  UMETA(DisplayName="None (passive / event-triggered)"),
	Attack1     = 1  UMETA(DisplayName="Attack 1 (Main Dish)"),
	Attack2     = 2  UMETA(DisplayName="Attack 2 (Appetizer)"),
	Dodge       = 3  UMETA(DisplayName="Dodge (Dessert)"),
	DrinkPotion = 4  UMETA(DisplayName="Drink Potion"),
};
