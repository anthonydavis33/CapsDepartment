#include "Dungeon/CapsDungeonGenerator.h"
#include "Dungeon/CapsRoomTemplate.h"
#include "Dungeon/DungeonScalingDataAsset.h"
#include "Dungeon/CapsTilesetDataAsset.h"
#include "Dungeon/CapsDoorActor.h"
#include "Enemies/CapsReinforcementComponent.h"
#include "Characters/CapsEnemyBase.h"
#include "Core/CapsGameSettings.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Algo/RandomShuffle.h"

ACapsDungeonGenerator::ACapsDungeonGenerator()
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	ReinforcementComponent = CreateDefaultSubobject<UCapsReinforcementComponent>(TEXT("ReinforcementComponent"));
}

void ACapsDungeonGenerator::BeginPlay()
{
	Super::BeginPlay();
}

void ACapsDungeonGenerator::StartRun()
{
	// Unbind delegates from any previous run's rooms
	for (auto& Pair : ActiveRoomTemplates)
	{
		if (ACapsRoomTemplate* Template = Pair.Value.Get())
		{
			for (ACapsDoorActor* Door : Template->Doors)
			{
				if (Door)
					Door->OnPlayerPassedThrough.RemoveDynamic(this, &ACapsDungeonGenerator::OnPlayerPassedThroughDoor);
			}
			Template->OnRoomCleared.RemoveDynamic(this, &ACapsDungeonGenerator::OnRoomCleared_Internal);
		}
	}

	RoomsCompleted    = 0;
	CurrentRoomIndex  = 0;
	CurrentTileset    = DefaultTileset;
	ActiveRoomTemplates.Empty();
	PendingLoads.Empty();
	KnownRoomTypes.Empty();
	VisitedRooms.Empty();
	VisitedRooms.Add(0); // player starts in room 0
	ReinforcementComponent->ResetForNewRun();

	ShufflePool();
	UpdateStreamedRooms();
}

void ACapsDungeonGenerator::ShufflePool()
{
	PoolSequence.SetNum(RoomPool.Num());
	for (int32 i = 0; i < RoomPool.Num(); ++i) PoolSequence[i] = i;
	Algo::RandomShuffle(PoolSequence);
}

FVector ACapsDungeonGenerator::GetRoomOrigin(int32 RoomIndex) const
{
	// Rooms extend East (+Y) from the generator's world location.
	return GetActorLocation() + FVector(0.f, RoomIndex * RoomSpacing, 0.f);
}

void ACapsDungeonGenerator::UpdateStreamedRooms()
{
	const int32 MinRoom = FMath::Max(0, CurrentRoomIndex - StreamRadius);
	const int32 MaxRoom = CurrentRoomIndex + StreamRadius;

	for (int32 i = MinRoom; i <= MaxRoom; ++i)
	{
		if (!ActiveRoomTemplates.Contains(i))
			LoadRoom(i);
	}

	TArray<int32> ToUnload;
	for (auto& Pair : ActiveRoomTemplates)
	{
		if (Pair.Key < MinRoom || Pair.Key > MaxRoom)
			ToUnload.Add(Pair.Key);
	}
	for (int32 Idx : ToUnload) UnloadRoom(Idx);
}

void ACapsDungeonGenerator::LoadRoom(int32 RoomIndex)
{
	if (RoomPool.IsEmpty()) return;

	const int32 PoolIdx = PoolSequence.IsEmpty()
		? 0
		: PoolSequence[RoomIndex % PoolSequence.Num()];

	bool bSuccess = false;
	ULevelStreamingDynamic* StreamingLevel = ULevelStreamingDynamic::LoadLevelInstanceBySoftObjectPtr(
		this,
		RoomPool[PoolIdx],
		GetRoomOrigin(RoomIndex),
		FRotator::ZeroRotator,
		bSuccess
	);

	if (!bSuccess || !StreamingLevel) return;

	PendingLoads.Emplace(StreamingLevel, RoomIndex);
	StreamingLevel->OnLevelLoaded.AddDynamic(this, &ACapsDungeonGenerator::OnRoomLevelLoaded);
}

void ACapsDungeonGenerator::UnloadRoom(int32 RoomIndex)
{
	if (ACapsRoomTemplate* Template = ActiveRoomTemplates.FindRef(RoomIndex))
	{
		for (ACapsDoorActor* Door : Template->Doors)
		{
			if (Door)
				Door->OnPlayerPassedThrough.RemoveDynamic(this, &ACapsDungeonGenerator::OnPlayerPassedThroughDoor);
		}
		Template->OnRoomCleared.RemoveDynamic(this, &ACapsDungeonGenerator::OnRoomCleared_Internal);
	}
	ActiveRoomTemplates.Remove(RoomIndex);
	// ULevelStreamingDynamic manages its own unload once no hard references remain.
}

void ACapsDungeonGenerator::OnRoomLevelLoaded()
{
	// Check all pending loads — multiple may resolve on the same frame.
	for (int32 i = PendingLoads.Num() - 1; i >= 0; --i)
	{
		ULevelStreamingDynamic* StreamingLevel = PendingLoads[i].Get<0>();
		const int32 RoomIndex                  = PendingLoads[i].Get<1>();

		if (!StreamingLevel)
		{
			PendingLoads.RemoveAt(i);
			continue;
		}

		ULevel* Level = StreamingLevel->GetLoadedLevel();
		if (!Level) continue; // still loading

		for (AActor* Actor : Level->Actors)
		{
			if (ACapsRoomTemplate* Template = Cast<ACapsRoomTemplate>(Actor))
			{
				ActivateRoom(Template, RoomIndex);
				PendingLoads.RemoveAt(i);
				break;
			}
		}
	}
}

void ACapsDungeonGenerator::ActivateRoom(ACapsRoomTemplate* Template, int32 RoomIndex)
{
	Template->RoomIndex = RoomIndex;
	ActiveRoomTemplates.Add(RoomIndex, Template);
	KnownRoomTypes.Add(RoomIndex, Template->RoomType);

	// West door is always open: room 0 exits back to hub, others back to previous room.
	// East door opens immediately for non-combat rooms; combat rooms open it on clear.
	TSet<ECardinalDirection> OpenDoors;
	OpenDoors.Add(ECardinalDirection::West);
	if (Template->RoomType != ERoomType::Combat)
		OpenDoors.Add(ECardinalDirection::East);

	Template->ConfigureDoors(OpenDoors);

	// Resolve tileset: room's own override wins, then generator's current, then default.
	UCapsTilesetDataAsset* Tileset = Template->DefaultTileset
		? Template->DefaultTileset.Get()
		: (CurrentTileset ? CurrentTileset.Get() : DefaultTileset.Get());

	if (Tileset)
		Template->ApplyTileset(Tileset);

	// Update the previous room's exit door to preview this room's tileset,
	// giving the player a visual hint before they walk through.
	if (RoomIndex > 0)
	{
		if (ACapsRoomTemplate* PrevRoom = ActiveRoomTemplates.FindRef(RoomIndex - 1))
		{
			if (ACapsDoorActor* PreviewDoor = PrevRoom->GetDoor(ECardinalDirection::East))
			{
				if (Tileset) PreviewDoor->ApplyTileset(Tileset);
			}
		}
	}

	for (ACapsDoorActor* Door : Template->Doors)
	{
		if (Door)
			Door->OnPlayerPassedThrough.AddDynamic(this, &ACapsDungeonGenerator::OnPlayerPassedThroughDoor);
	}
	Template->OnRoomCleared.AddDynamic(this, &ACapsDungeonGenerator::OnRoomCleared_Internal);
}

void ACapsDungeonGenerator::SetTileset(UCapsTilesetDataAsset* NewTileset)
{
	CurrentTileset = NewTileset;

	// Update the current room's exit door so the player can see the theme change
	// before stepping into the next room.
	if (ACapsRoomTemplate* Current = ActiveRoomTemplates.FindRef(CurrentRoomIndex))
	{
		if (ACapsDoorActor* EastDoor = Current->GetDoor(ECardinalDirection::East))
		{
			UCapsTilesetDataAsset* Tileset = NewTileset ? NewTileset : DefaultTileset.Get();
			if (Tileset) EastDoor->ApplyTileset(Tileset);
		}
	}
}

void ACapsDungeonGenerator::OnPlayerPassedThroughDoor(ECardinalDirection Direction)
{
	if (Direction == ECardinalDirection::East)
	{
		++CurrentRoomIndex;
		++RoomsCompleted;
		VisitedRooms.Add(CurrentRoomIndex);
		UpdateStreamedRooms();
	}
	else if (Direction == ECardinalDirection::West && CurrentRoomIndex > 0)
	{
		--CurrentRoomIndex;
		VisitedRooms.Add(CurrentRoomIndex);
		UpdateStreamedRooms();
	}
	// West at room 0 leads back to hub — the hub level owns that transition.
}

void ACapsDungeonGenerator::OnRoomCleared_Internal()
{
	ACapsRoomTemplate* Template = ActiveRoomTemplates.FindRef(CurrentRoomIndex);
	if (!Template) return;

	if (ACapsDoorActor* EastDoor = Template->GetDoor(ECardinalDirection::East))
		EastDoor->SetState(EDoorState::Open);

	// Safe rooms never generate reinforcements
	if (Template->RoomType == ERoomType::Combat)
		ReinforcementComponent->NotifyRoomCleared(CurrentRoomIndex);
}

ACapsRoomTemplate* ACapsDungeonGenerator::GetCurrentRoomTemplate() const
{
	if (const TObjectPtr<ACapsRoomTemplate>* Found = ActiveRoomTemplates.Find(CurrentRoomIndex))
		return Found->Get();
	return nullptr;
}

TArray<FMinimapRoomEntry> ACapsDungeonGenerator::GetMinimapData() const
{
	const int32 RevealRange = UCapsGameSettings::Get()->SafeRoomRevealRange;

	TArray<FMinimapRoomEntry> Result;
	for (const auto& Pair : KnownRoomTypes)
	{
		const int32 RoomIdx   = Pair.Key;
		const ERoomType RType = Pair.Value;

		const bool bVisited   = VisitedRooms.Contains(RoomIdx);
		const bool bIsCurrent = (RoomIdx == CurrentRoomIndex);
		const bool bNearby    = (RType == ERoomType::Safe)
			&& !bVisited
			&& (FMath::Abs(RoomIdx - CurrentRoomIndex) <= RevealRange);

		if (!bVisited && !bIsCurrent && !bNearby)
			continue;

		FMinimapRoomEntry& Entry       = Result.AddDefaulted_GetRef();
		Entry.RoomIndex                = RoomIdx;
		Entry.RoomType                 = RType;
		Entry.bVisited                 = bVisited;
		Entry.bIsCurrent               = bIsCurrent;
		Entry.bRevealedByProximity     = bNearby;
	}

	Result.Sort([](const FMinimapRoomEntry& A, const FMinimapRoomEntry& B)
	{
		return A.RoomIndex < B.RoomIndex;
	});

	// Populate enemy dots — gather all live enemies once, then bin by room
	TArray<AActor*> EnemyActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACapsEnemyBase::StaticClass(), EnemyActors);

	const float HalfSpacing = RoomSpacing * 0.5f;
	for (FMinimapRoomEntry& Entry : Result)
	{
		const FVector RoomCenter = GetRoomOrigin(Entry.RoomIndex);
		for (AActor* Actor : EnemyActors)
		{
			const ACapsEnemyBase* Enemy = Cast<ACapsEnemyBase>(Actor);
			if (!Enemy || !Enemy->IsAlive()) continue;

			const FVector Rel = Enemy->GetActorLocation() - RoomCenter;
			if (FMath::Abs(Rel.X) > HalfSpacing || FMath::Abs(Rel.Y) > HalfSpacing) continue;

			Entry.EnemyPositions.Add(FVector2D(
				FMath::Clamp(Rel.X / HalfSpacing, -1.f, 1.f),
				FMath::Clamp(Rel.Y / HalfSpacing, -1.f, 1.f)
			));
		}
	}

	return Result;
}

const FDungeonScalingTier& ACapsDungeonGenerator::GetCurrentScalingTier() const
{
	static const FDungeonScalingTier DefaultTier;
	if (!ScalingData) return DefaultTier;
	return ScalingData->GetTierForRoom(RoomsCompleted);
}
