#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayAbilitySpec.h"
#include "CapsSecondaryWeaponComponent.generated.h"

class UCapsGameplayAbility;
class UCapsAbilitySystemComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSecondaryWeaponChanged,
	TSubclassOf<UCapsGameplayAbility>, NewWeaponClass);

// Manages the character's swappable secondary weapon.
// Lives on ACapsCharacter alongside the ASC.
//
// Equip flow (run start):
//   EquipSecondary(CharacterData->DefaultSecondaryAbility)
//
// Swap flow (weapon pickup actor):
//   SwapSecondaryWeapon(NewWeaponClass)
//   → removes old Attack2 ability spec from ASC
//   → grants new one with InputID = Attack2
//   → fires OnSecondaryWeaponChanged for HUD update
//
// Food modifier note: food effects live on the CHARACTER's ASC (applied by EatMeal).
// They are NOT tied to the weapon spec, so they survive any weapon swap automatically.
UCLASS(ClassGroup=Caps, meta=(BlueprintSpawnableComponent))
class CAPS_API UCapsSecondaryWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Fired when the secondary weapon changes. Bind in HUD Blueprint to refresh the icon.
	UPROPERTY(BlueprintAssignable, Category="Weapon")
	FOnSecondaryWeaponChanged OnSecondaryWeaponChanged;

	// Returns the currently equipped secondary weapon class (may be null before equip).
	UFUNCTION(BlueprintPure, Category="Weapon")
	TSubclassOf<UCapsGameplayAbility> GetCurrentWeaponClass() const { return CurrentWeaponClass; }

	// Called at run start with the character's default secondary from their data asset.
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void EquipSecondary(TSubclassOf<UCapsGameplayAbility> WeaponClass);

	// Called by weapon pickup actors mid-run. Removes old spec, grants new one.
	// Food modifier effects on the character are unaffected.
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void SwapSecondaryWeapon(TSubclassOf<UCapsGameplayAbility> NewWeaponClass);

	// Removes the secondary weapon entirely (e.g. on run end / cleanup).
	UFUNCTION(BlueprintCallable, Category="Weapon")
	void ClearSecondary();

private:
	TSubclassOf<UCapsGameplayAbility> CurrentWeaponClass;
	FGameplayAbilitySpecHandle        CurrentWeaponHandle;

	UCapsAbilitySystemComponent* GetOwnerASC() const;
};
