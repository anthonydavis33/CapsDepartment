// Copyright Epic Games, Inc. All Rights Reserved.

#include "CapsCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Abilities/CapsAbilitySystemComponent.h"
#include "Abilities/CapsAttributeSet.h"
#include "Items/CapsInventoryComponent.h"

ACapsCharacter::ACapsCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 640.f, 0.f);
	GetCharacterMovement()->bConstrainToPlane = true;
	GetCharacterMovement()->bSnapToPlaneAtStart = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(true);
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false;

	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false;

	AbilitySystemComponent = CreateDefaultSubobject<UCapsAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);

	// AttributeSet registers itself with the ASC automatically on construction
	AttributeSet = CreateDefaultSubobject<UCapsAttributeSet>(TEXT("AttributeSet"));

	InventoryComponent = CreateDefaultSubobject<UCapsInventoryComponent>(TEXT("InventoryComponent"));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ACapsCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void ACapsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

UAbilitySystemComponent* ACapsCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}
