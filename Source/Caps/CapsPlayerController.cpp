// Copyright Epic Games, Inc. All Rights Reserved.

#include "CapsPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Pawn.h"
#include "Abilities/CapsAbilitySystemComponent.h"
#include "CapsCharacter.h"
#include "Caps.h"

ACapsPlayerController::ACapsPlayerController()
{
	bShowMouseCursor = false;
}

void ACapsPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		Subsystem->AddMappingContext(DefaultMappingContext, 0);
	}

	if (UEnhancedInputComponent* EIC = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EIC->BindAction(MoveAction,        ETriggerEvent::Triggered,  this, &ACapsPlayerController::OnMove);

		EIC->BindAction(Attack1Action,     ETriggerEvent::Started,    this, &ACapsPlayerController::OnAttack1);
		EIC->BindAction(Attack1Action,     ETriggerEvent::Completed,  this, &ACapsPlayerController::OnAttack1Released);

		EIC->BindAction(Attack2Action,     ETriggerEvent::Started,    this, &ACapsPlayerController::OnAttack2);
		EIC->BindAction(Attack2Action,     ETriggerEvent::Completed,  this, &ACapsPlayerController::OnAttack2Released);

		EIC->BindAction(DodgeAction,       ETriggerEvent::Started,    this, &ACapsPlayerController::OnDodge);
		EIC->BindAction(DodgeAction,       ETriggerEvent::Completed,  this, &ACapsPlayerController::OnDodgeReleased);

		EIC->BindAction(DrinkPotionAction, ETriggerEvent::Started,    this, &ACapsPlayerController::OnDrinkPotion);
		EIC->BindAction(DrinkPotionAction, ETriggerEvent::Completed,  this, &ACapsPlayerController::OnDrinkPotionReleased);
	}
	else
	{
		UE_LOG(LogCaps, Error, TEXT("%s: Failed to find EnhancedInputComponent."), *GetNameSafe(this));
	}
}

void ACapsPlayerController::OnMove(const FInputActionValue& Value)
{
	const FVector2D Axis = Value.Get<FVector2D>();
	if (APawn* ControlledPawn = GetPawn())
	{
		// Camera is fixed overhead — world axes map directly to screen directions.
		ControlledPawn->AddMovementInput(FVector::ForwardVector, Axis.Y);
		ControlledPawn->AddMovementInput(FVector::RightVector,   Axis.X);
	}
}

UCapsAbilitySystemComponent* ACapsPlayerController::GetPlayerASC() const
{
	if (const ACapsCharacter* Char = Cast<ACapsCharacter>(GetPawn()))
		return Cast<UCapsAbilitySystemComponent>(Char->GetAbilitySystemComponent());
	return nullptr;
}

void ACapsPlayerController::OnAttack1()
{
	UCapsAbilitySystemComponent* ASC = GetPlayerASC();
	UE_LOG(LogTemp, Warning, TEXT("OnAttack1 fired — ASC: %s"), ASC ? TEXT("valid") : TEXT("NULL"));
	if (ASC)
		ASC->PressInputID(ECapsAbilityInputID::Attack1);
}

void ACapsPlayerController::OnAttack1Released()
{
	if (UCapsAbilitySystemComponent* ASC = GetPlayerASC())
		ASC->ReleaseInputID(ECapsAbilityInputID::Attack1);
}

void ACapsPlayerController::OnAttack2()
{
	if (UCapsAbilitySystemComponent* ASC = GetPlayerASC())
		ASC->PressInputID(ECapsAbilityInputID::Attack2);
}

void ACapsPlayerController::OnAttack2Released()
{
	if (UCapsAbilitySystemComponent* ASC = GetPlayerASC())
		ASC->ReleaseInputID(ECapsAbilityInputID::Attack2);
}

void ACapsPlayerController::OnDodge()
{
	if (UCapsAbilitySystemComponent* ASC = GetPlayerASC())
		ASC->PressInputID(ECapsAbilityInputID::Dodge);
}

void ACapsPlayerController::OnDodgeReleased()
{
	if (UCapsAbilitySystemComponent* ASC = GetPlayerASC())
		ASC->ReleaseInputID(ECapsAbilityInputID::Dodge);
}

void ACapsPlayerController::OnDrinkPotion()
{
	if (UCapsAbilitySystemComponent* ASC = GetPlayerASC())
		ASC->PressInputID(ECapsAbilityInputID::DrinkPotion);
}

void ACapsPlayerController::OnDrinkPotionReleased()
{
	if (UCapsAbilitySystemComponent* ASC = GetPlayerASC())
		ASC->ReleaseInputID(ECapsAbilityInputID::DrinkPotion);
}
