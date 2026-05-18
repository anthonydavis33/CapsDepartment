#include "Characters/CapsSecondaryWeaponComponent.h"
#include "Abilities/CapsAbilitySystemComponent.h"
#include "AbilitySystemInterface.h"

UCapsAbilitySystemComponent* UCapsSecondaryWeaponComponent::GetOwnerASC() const
{
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetOwner());
	return ASI ? Cast<UCapsAbilitySystemComponent>(ASI->GetAbilitySystemComponent()) : nullptr;
}

void UCapsSecondaryWeaponComponent::EquipSecondary(TSubclassOf<UCapsGameplayAbility> WeaponClass)
{
	if (!WeaponClass) return;

	UCapsAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC) return;

	CurrentWeaponClass  = WeaponClass;
	CurrentWeaponHandle = ASC->GrantAbility(WeaponClass);

	OnSecondaryWeaponChanged.Broadcast(CurrentWeaponClass);
}

void UCapsSecondaryWeaponComponent::SwapSecondaryWeapon(TSubclassOf<UCapsGameplayAbility> NewWeaponClass)
{
	if (!NewWeaponClass || NewWeaponClass == CurrentWeaponClass) return;

	UCapsAbilitySystemComponent* ASC = GetOwnerASC();
	if (!ASC) return;

	// Remove the current secondary spec before granting the new one.
	if (CurrentWeaponHandle.IsValid())
		ASC->ClearAbility(CurrentWeaponHandle);

	CurrentWeaponClass  = NewWeaponClass;
	CurrentWeaponHandle = ASC->GrantAbility(NewWeaponClass);

	OnSecondaryWeaponChanged.Broadcast(CurrentWeaponClass);
}

void UCapsSecondaryWeaponComponent::ClearSecondary()
{
	if (!CurrentWeaponHandle.IsValid()) return;

	if (UCapsAbilitySystemComponent* ASC = GetOwnerASC())
		ASC->ClearAbility(CurrentWeaponHandle);

	CurrentWeaponHandle = FGameplayAbilitySpecHandle();
	CurrentWeaponClass  = nullptr;
}
