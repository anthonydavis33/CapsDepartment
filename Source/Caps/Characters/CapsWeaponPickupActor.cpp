#include "Characters/CapsWeaponPickupActor.h"
#include "Characters/CapsSecondaryWeaponComponent.h"
#include "Abilities/CapsGameplayAbility.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

ACapsWeaponPickupActor::ACapsWeaponPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	OverlapVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapVolume"));
	OverlapVolume->SetupAttachment(RootComponent);
	OverlapVolume->SetBoxExtent(FVector(60.f, 60.f, 60.f));
	OverlapVolume->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
}

void ACapsWeaponPickupActor::BeginPlay()
{
	Super::BeginPlay();
	OverlapVolume->OnComponentBeginOverlap.AddDynamic(this, &ACapsWeaponPickupActor::OnOverlapBegin);
}

void ACapsWeaponPickupActor::OnOverlapBegin(UPrimitiveComponent* /*OverlappedComp*/,
	AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	if (bAlreadyPickedUp || !WeaponClass) return;

	ACharacter* Picker = Cast<ACharacter>(OtherActor);
	if (!Picker) return;

	UCapsSecondaryWeaponComponent* SecondaryComp =
		Picker->FindComponentByClass<UCapsSecondaryWeaponComponent>();
	if (!SecondaryComp) return;

	bAlreadyPickedUp = true;

	// Spawn a pickup for the weapon the player is dropping, at this actor's location.
	TSubclassOf<ACapsWeaponPickupActor> DropClass =
		SpawnedPickupClass ? SpawnedPickupClass : GetClass();

	const TSubclassOf<UCapsGameplayAbility> DroppedWeaponClass = SecondaryComp->GetCurrentWeaponClass();

	if (DroppedWeaponClass)
	{
		FActorSpawnParameters Params;
		Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		ACapsWeaponPickupActor* DroppedPickup = GetWorld()->SpawnActor<ACapsWeaponPickupActor>(
			DropClass, GetActorLocation() + FVector(0.f, 0.f, 20.f),
			FRotator::ZeroRotator, Params);
		if (DroppedPickup)
			DroppedPickup->WeaponClass = DroppedWeaponClass;
	}

	// Equip the new weapon (food effects on the character remain untouched).
	SecondaryComp->SwapSecondaryWeapon(WeaponClass);

	BP_OnPickedUp(Picker);
	Destroy();
}
