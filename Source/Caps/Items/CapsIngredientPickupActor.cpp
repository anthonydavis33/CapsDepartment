#include "Items/CapsIngredientPickupActor.h"
#include "Items/CapsInventoryComponent.h"
#include "CapsCharacter.h"
#include "Components/SphereComponent.h"

ACapsIngredientPickupActor::ACapsIngredientPickupActor()
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionSphere"));
	CollisionSphere->InitSphereRadius(60.f);
	CollisionSphere->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
	RootComponent = CollisionSphere;
}

void ACapsIngredientPickupActor::BeginPlay()
{
	Super::BeginPlay();
	CollisionSphere->OnComponentBeginOverlap.AddDynamic(
		this, &ACapsIngredientPickupActor::OnOverlapBegin);
}

void ACapsIngredientPickupActor::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
	bool bFromSweep, const FHitResult& SweepResult)
{
	ACapsCharacter* Character = Cast<ACapsCharacter>(OtherActor);
	if (!Character || !Character->IsAlive()) return;

	if (!IngredientID.IsNone())
	{
		if (UCapsInventoryComponent* Inventory = Character->GetInventoryComponent())
			Inventory->AddRunPickup(IngredientID, Quantity);
	}

	BP_OnPickedUp(Character);

	// Destroy after the BP event so VFX/sound have a chance to fire.
	// If the BP implementation needs more time, it should detach the VFX first.
	Destroy();
}
