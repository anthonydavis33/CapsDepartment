// Copyright Epic Games, Inc. All Rights Reserved.

#include "CapsPlayerController.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/Pawn.h"
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
		EIC->BindAction(MoveAction,        ETriggerEvent::Triggered, this, &ACapsPlayerController::OnMove);
		EIC->BindAction(Attack1Action,     ETriggerEvent::Started,   this, &ACapsPlayerController::OnAttack1);
		EIC->BindAction(Attack2Action,     ETriggerEvent::Started,   this, &ACapsPlayerController::OnAttack2);
		EIC->BindAction(DodgeAction,       ETriggerEvent::Started,   this, &ACapsPlayerController::OnDodge);
		EIC->BindAction(DrinkPotionAction, ETriggerEvent::Started,   this, &ACapsPlayerController::OnDrinkPotion);
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

void ACapsPlayerController::OnAttack1()
{
	UE_LOG(LogCaps, Log, TEXT("Attack1 — stub"));
}

void ACapsPlayerController::OnAttack2()
{
	UE_LOG(LogCaps, Log, TEXT("Attack2 — stub"));
}

void ACapsPlayerController::OnDodge()
{
	UE_LOG(LogCaps, Log, TEXT("Dodge — stub"));
}

void ACapsPlayerController::OnDrinkPotion()
{
	UE_LOG(LogCaps, Log, TEXT("DrinkPotion — stub"));
}
