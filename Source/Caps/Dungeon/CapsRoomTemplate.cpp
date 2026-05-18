#include "Dungeon/CapsRoomTemplate.h"
#include "Dungeon/CapsDoorActor.h"
#include "Dungeon/CapsTilesetDataAsset.h"
#include "Dungeon/CapsRoomTileComponent.h"
#include "Components/StaticMeshComponent.h"

ACapsRoomTemplate::ACapsRoomTemplate()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
}

void ACapsRoomTemplate::BeginPlay()
{
	Super::BeginPlay();
}

void ACapsRoomTemplate::ConfigureDoors(const TSet<ECardinalDirection>& OpenDirections)
{
	for (ACapsDoorActor* Door : Doors)
	{
		if (!Door) continue;
		const EDoorState NewState = OpenDirections.Contains(Door->Direction)
			? EDoorState::Open
			: EDoorState::Sealed;
		Door->SetState(NewState);
	}
}

ACapsDoorActor* ACapsRoomTemplate::GetDoor(ECardinalDirection Direction) const
{
	for (ACapsDoorActor* Door : Doors)
	{
		if (Door && Door->Direction == Direction)
			return Door;
	}
	return nullptr;
}

void ACapsRoomTemplate::NotifyRoomCleared()
{
	bWasCleared = true;
	OnRoomCleared.Broadcast();
}

void ACapsRoomTemplate::LockForReinforcements(int32 EnemyCount)
{
	ActiveReinforcementCount += EnemyCount;

	if (ACapsDoorActor* EastDoor = GetDoor(ECardinalDirection::East))
		EastDoor->SetState(EDoorState::Sealed);
}

void ACapsRoomTemplate::OnReinforcementEnemyDied()
{
	ActiveReinforcementCount = FMath::Max(0, ActiveReinforcementCount - 1);

	if (ActiveReinforcementCount == 0 && bWasCleared)
	{
		if (ACapsDoorActor* EastDoor = GetDoor(ECardinalDirection::East))
			EastDoor->SetState(EDoorState::Open);
	}
}

void ACapsRoomTemplate::ApplyTileset(UCapsTilesetDataAsset* Tileset)
{
	if (!Tileset) return;

	// Swap meshes on every tile actor in this Level Instance.
	ULevel* Level = GetLevel();
	if (Level)
	{
		for (AActor* Actor : Level->Actors)
		{
			if (!Actor) continue;
			UCapsRoomTileComponent* TileComp = Actor->FindComponentByClass<UCapsRoomTileComponent>();
			if (!TileComp) continue;

			UStaticMeshComponent* SMC = Actor->FindComponentByClass<UStaticMeshComponent>();
			if (!SMC) continue;

			if (UStaticMesh* Mesh = Tileset->GetMeshForSlot(TileComp->SlotType))
				SMC->SetStaticMesh(Mesh);

			if (UMaterialInterface* Mat = Tileset->GetMaterialForSlot(TileComp->SlotType))
				SMC->SetMaterial(0, Mat);
		}
	}

	// Apply door mesh to all doors in this room.
	for (ACapsDoorActor* Door : Doors)
	{
		if (Door) Door->ApplyTileset(Tileset);
	}

	OnTilesetApplied(Tileset);
}
