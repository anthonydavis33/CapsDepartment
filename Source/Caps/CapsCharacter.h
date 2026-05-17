// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "CapsCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UCapsAbilitySystemComponent;
class UCapsAttributeSet;

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

public:
	ACapsCharacter();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UCapsAttributeSet* GetAttributeSet() const { return AttributeSet.Get(); }
	UCameraComponent* GetTopDownCameraComponent() const { return TopDownCameraComponent.Get(); }
	USpringArmComponent* GetCameraBoom() const { return CameraBoom.Get(); }
};
