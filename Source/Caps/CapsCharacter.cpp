// Copyright Epic Games, Inc. All Rights Reserved.

#include "CapsCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Abilities/CapsAbilitySystemComponent.h"
#include "Abilities/CapsAttributeSet.h"
#include "Abilities/CapsGameplayAbility.h"
#include "Characters/CapsSecondaryWeaponComponent.h"
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

	// Don't carve a hole in the navmesh — enemies need to path to the player's location.
	GetCapsuleComponent()->SetCanEverAffectNavigation(false);

	AbilitySystemComponent = CreateDefaultSubobject<UCapsAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);

	// AttributeSet registers itself with the ASC automatically on construction
	AttributeSet = CreateDefaultSubobject<UCapsAttributeSet>(TEXT("AttributeSet"));

	InventoryComponent = CreateDefaultSubobject<UCapsInventoryComponent>(TEXT("InventoryComponent"));

	SecondaryWeaponComponent = CreateDefaultSubobject<UCapsSecondaryWeaponComponent>(TEXT("SecondaryWeaponComponent"));

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ACapsCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UCapsAttributeSet::GetHealthAttribute())
			.AddUObject(this, &ACapsCharacter::OnHealthChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UCapsAttributeSet::GetMoveSpeedAttribute())
			.AddUObject(this, &ACapsCharacter::OnMoveSpeedChanged);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
			UCapsAttributeSet::GetAttackDamageAttribute())
			.AddUObject(this, &ACapsCharacter::OnAttackDamageChanged);

		GrantDefaultAbilities();

		UE_LOG(LogTemp, Warning, TEXT("CapsCharacter BeginPlay: ASC init done. DefaultAbilities count: %d"), DefaultAbilities.Num());
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("CapsCharacter BeginPlay: AbilitySystemComponent is NULL!"));
	}
}

void ACapsCharacter::GrantDefaultAbilities()
{
	for (const TSubclassOf<UCapsGameplayAbility>& AbilityClass : DefaultAbilities)
	{
		FGameplayAbilitySpecHandle Handle = AbilitySystemComponent->GrantAbility(AbilityClass);
		UE_LOG(LogTemp, Warning, TEXT("GrantDefaultAbilities: %s → handle valid: %d"),
			AbilityClass ? *AbilityClass->GetName() : TEXT("NULL"), Handle.IsValid());
	}
}

void ACapsCharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			static_cast<int32>(GetUniqueID()),   // stable key per actor — overwrites the previous line for this actor
			3.f,
			FColor::Green,
			FString::Printf(TEXT("%s HP: %.0f"), *GetName(), Data.NewValue));
	}

	if (bIsDead || Data.NewValue > 0.f) return;
	HandleDeath();
}

void ACapsCharacter::OnMoveSpeedChanged(const FOnAttributeChangeData& Data)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			static_cast<int32>(GetUniqueID()) + 1,
			3.f, FColor::Cyan,
			FString::Printf(TEXT("%s Speed: %.0f"), *GetName(), Data.NewValue));
	}
}

void ACapsCharacter::OnAttackDamageChanged(const FOnAttributeChangeData& Data)
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(
			static_cast<int32>(GetUniqueID()) + 2,
			3.f, FColor::Orange,
			FString::Printf(TEXT("%s ATK: %.0f"), *GetName(), Data.NewValue));
	}
}

void ACapsCharacter::HandleDeath()
{
	bIsDead = true;

	// Block all ability activation immediately.
	if (AbilitySystemComponent)
		AbilitySystemComponent->AddLooseGameplayTag(
			FGameplayTag::RequestGameplayTag(FName("State.Dead")));

	if (InventoryComponent)
		InventoryComponent->HandleDeath();

	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BP_OnDeath();
}

void ACapsCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
}

UAbilitySystemComponent* ACapsCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}
