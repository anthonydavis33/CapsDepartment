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

public:
	ACapsCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UCapsAttributeSet* GetAttributeSet() const { return AttributeSet.Get(); }
	UCapsInventoryComponent* GetInventoryComponent() const { return InventoryComponent.Get(); }
	UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent.Get(); }
	USpringArmComponent* GetCameraBoom() const { return CameraBoom.Get(); }

	bool IsAlive() const { return !bIsDead; }

	// Abilities to grant when the character spawns. Assign Blueprint ability
	// subclasses here in the player's Blueprint Details panel.
	// Each ability's InputID determines which action triggers it.
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
