#include "Dungeon/CapsDoorActor.h"
#include "Dungeon/CapsTilesetDataAsset.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Character.h"
#include "Caps.h"

ACapsDoorActor::ACapsDoorActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);
	TriggerVolume->SetBoxExtent(FVector(100.f, 100.f, 96.f));
	TriggerVolume->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);
}

void ACapsDoorActor::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ACapsDoorActor::OnOverlapBegin);
	UpdateVisuals();
}

void ACapsDoorActor::SetState(EDoorState NewState)
{
	State = NewState;
	UpdateVisuals();
	OnStateChanged(NewState);
}

void ACapsDoorActor::OnOverlapBegin(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (State != EDoorState::Open) return;
	if (!Cast<ACharacter>(OtherActor)) return;

	OnPlayerPassedThrough.Broadcast(Direction);
}

void ACapsDoorActor::ApplyTileset(UCapsTilesetDataAsset* Tileset)
{
	if (!Tileset || !DoorMesh) return;
	if (UStaticMesh* Mesh = Tileset->DoorMesh.Get())
		DoorMesh->SetStaticMesh(Mesh);
	OnTilesetApplied(Tileset);
}

void ACapsDoorActor::UpdateVisuals()
{
	// Sealed doors block passage visually. Placeholder until art is added.
	if (DoorMesh)
	{
		DoorMesh->SetVisibility(State == EDoorState::Sealed);
	}
}
