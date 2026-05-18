#include "Dungeon/CapsHubDoorActor.h"
#include "Core/CapsGameMode.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"

ACapsHubDoorActor::ACapsHubDoorActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);

	SealMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SealMesh"));
	SealMesh->SetupAttachment(RootComponent);
	SealMesh->SetVisibility(false); // hidden until sealed

	EntryVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("EntryVolume"));
	EntryVolume->SetupAttachment(RootComponent);
	EntryVolume->SetBoxExtent(FVector(80.f, 120.f, 100.f));
	EntryVolume->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
}

void ACapsHubDoorActor::BeginPlay()
{
	Super::BeginPlay();
	EntryVolume->OnComponentBeginOverlap.AddDynamic(this, &ACapsHubDoorActor::OnEntryOverlap);
}

void ACapsHubDoorActor::OnEntryOverlap(UPrimitiveComponent* /*OverlappedComp*/,
	AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/,
	int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	if (bIsSealed || bTransitionFired) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || PC->GetPawn() != OtherActor) return;

	bTransitionFired = true;

	// Disable player input for the cinematic walk-through.
	PC->SetInputMode(FInputModeUIOnly());

	BP_OnPlayerEntered();
	// Blueprint calls CommitTransition() when the cinematic finishes.
}

void ACapsHubDoorActor::CommitTransition()
{
	SealDoor();

	ACapsGameMode* GM = Cast<ACapsGameMode>(UGameplayStatics::GetGameMode(this));
	if (GM) GM->BeginDungeonTransition();
}

void ACapsHubDoorActor::SealDoor()
{
	if (bIsSealed) return;
	bIsSealed = true;
	SealMesh->SetVisibility(true);
	BP_OnSealed();
}
