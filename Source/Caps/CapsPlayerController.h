// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "Abilities/CapsAbilityInputID.h"
#include "CapsPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class UCapsAbilitySystemComponent;

UCLASS(abstract)
class ACapsPlayerController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> Attack1Action;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> Attack2Action;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> DodgeAction;

	UPROPERTY(EditAnywhere, Category="Input")
	TObjectPtr<UInputAction> DrinkPotionAction;

public:
	ACapsPlayerController();

protected:
	virtual void SetupInputComponent() override;

private:
	void OnMove(const FInputActionValue& Value);
	void OnAttack1();
	void OnAttack2();
	void OnDodge();
	void OnDrinkPotion();

	// For held abilities: fires AbilityLocalInputReleased when the key comes up.
	void OnAttack1Released();
	void OnAttack2Released();
	void OnDodgeReleased();
	void OnDrinkPotionReleased();

	UCapsAbilitySystemComponent* GetPlayerASC() const;
};
