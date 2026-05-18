#include "Enemies/CapsReinforcementComponent.h"
#include "Characters/CapsEnemyBase.h"
#include "Dungeon/CapsDungeonGenerator.h"
#include "Dungeon/CapsRoomTemplate.h"
#include "Dungeon/CapsDoorActor.h"
#include "Core/CapsGameSettings.h"

UCapsReinforcementComponent::UCapsReinforcementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false; // enabled on first room clear
}

ACapsDungeonGenerator* UCapsReinforcementComponent::GetGenerator() const
{
	return Cast<ACapsDungeonGenerator>(GetOwner());
}

void UCapsReinforcementComponent::ResetForNewRun()
{
	ActiveSources.Empty();
	SetComponentTickEnabled(false);
}

void UCapsReinforcementComponent::NotifyRoomCleared(int32 RoomIndex)
{
	const UCapsGameSettings* Settings = UCapsGameSettings::Get();

	FReinforcementSource& Source = ActiveSources.AddDefaulted_GetRef();
	Source.RoomIndex     = RoomIndex;
	Source.TimeUntilWave = Settings->ReinforcementInitialDelay;
	Source.WavesSent     = 0;

	SetComponentTickEnabled(true);
}

void UCapsReinforcementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	for (FReinforcementSource& Source : ActiveSources)
	{
		Source.TimeUntilWave -= DeltaTime;
		if (Source.TimeUntilWave <= 0.f)
		{
			const UCapsGameSettings* Settings = UCapsGameSettings::Get();
			if (FMath::FRand() <= Settings->ReinforcementSpawnChance)
			{
				SpawnWave(Source);
				Source.WavesSent++;
			}
			Source.TimeUntilWave = Settings->ReinforcementWaveInterval;
		}
	}
}

void UCapsReinforcementComponent::SpawnWave(FReinforcementSource& Source)
{
	ACapsDungeonGenerator* Generator = GetGenerator();
	if (!Generator || !ReinforcementEnemyClass) return;

	ACapsRoomTemplate* CurrentRoom = Generator->GetCurrentRoomTemplate();
	if (!CurrentRoom) return;

	const UCapsGameSettings* Settings = UCapsGameSettings::Get();
	const int32 EnemyCount = FMath::CeilToInt(
		Settings->BaseReinforcementsPerWave +
		Generator->GetRoomsCompleted() * Settings->ReinforcementsPerRoomCleared
	);

	// Notify Blueprint so a "Reinforcements incoming!" warning can be shown
	OnWaveIncoming.Broadcast();

	// Lock the current room's exit
	CurrentRoom->LockForReinforcements(EnemyCount);

	// Spawn near the West door — enemies arrive from cleared rooms behind the player
	FVector SpawnBase = CurrentRoom->GetActorLocation();
	if (ACapsDoorActor* WestDoor = CurrentRoom->GetDoor(ECardinalDirection::West))
		SpawnBase = WestDoor->GetActorLocation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	for (int32 i = 0; i < EnemyCount; ++i)
	{
		// Spread spawns so enemies don't stack on the same point
		const FVector SpawnLoc = SpawnBase + FVector(
			FMath::RandRange(-120.f, 120.f),
			FMath::RandRange(-120.f, 120.f),
			0.f
		);

		ACapsEnemyBase* Enemy = GetWorld()->SpawnActor<ACapsEnemyBase>(
			ReinforcementEnemyClass, SpawnLoc, FRotator::ZeroRotator, SpawnParams);

		if (Enemy)
		{
			Enemy->ApplyScaling(Generator->GetCurrentScalingTier());
			Enemy->OnEnemyDied.AddDynamic(this, &UCapsReinforcementComponent::OnReinforcementEnemyDied);
		}
	}
}

void UCapsReinforcementComponent::OnReinforcementEnemyDied(ACapsEnemyBase* Enemy)
{
	ACapsDungeonGenerator* Generator = GetGenerator();
	if (!Generator) return;

	if (ACapsRoomTemplate* CurrentRoom = Generator->GetCurrentRoomTemplate())
		CurrentRoom->OnReinforcementEnemyDied();
}
