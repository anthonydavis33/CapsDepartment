// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "CapsCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UCapsAbilitySystemComponent;
class UCapsAttributeSet;
class UCapsInventoryComponent;
class UCapsGameplayAbility;
class UCapsSecondaryWeaponComponent;

UCLASS(abstract)
class ACapsCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCameraComponent> TopDownCameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCapsAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UCapsAttributeSet> AttributeSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Inventory", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCapsInventoryComponent> InventoryComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCapsSecondaryWeaponComponent> SecondaryWeaponComponent;

public:
	ACapsCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UCapsAttributeSet* GetAttributeSet() const { return AttributeSet.Get(); }
	UCapsInventoryComponent* GetInventoryComponent() const { return InventoryComponent.Get(); }
	UCapsSecondaryWeaponComponent* GetSecondaryWeaponComponent() const { return SecondaryWeaponComponent.Get(); }
	UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent.Get(); }
	USpringArmComponent* GetCameraBoom() const { return CameraBoom.Get(); }

	bool IsAlive() const { return !bIsDead; }

	// Generic abilities granted regardless of which character is selected
	// (utility abilities, dodge roll, etc.). Character-specific abilities
	// come from UCapsCharacterDataAsset and are applied by ACapsGameMode.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abilities")
	TArray<TSubclassOf<UCapsGameplayAbility>> DefaultAbilities;

protected:
	// Override in Blueprint to play death screen, trigger respawn, or transition to hub.
	UFUNCTION(BlueprintImplementableEvent, Category="Character")
	void BP_OnDeath();

private:
	bool bIsDead = false;

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void HandleDeath();
	void GrantDefaultAbilities();
};
