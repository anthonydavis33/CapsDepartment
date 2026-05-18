#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CapsReinforcementComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWaveIncoming);

class ACapsEnemyBase;
class ACapsDungeonGenerator;

// One cleared room acting as a persistent wave source.
struct FReinforcementSource
{
	int32 RoomIndex       = 0;
	float TimeUntilWave   = 0.f; // countdown in seconds
	int32 WavesSent       = 0;
};

// Lives on ACapsDungeonGenerator. Each time a room clears it registers a new
// source here. Sources tick down independently and fire waves at the player's
// current room, sealing the exit until the wave is cleared.
//
// All timing and scaling values come from UCapsGameSettings so designers can
// tune pressure without recompiling.
UCLASS(ClassGroup=Caps, meta=(BlueprintSpawnableComponent))
class CAPS_API UCapsReinforcementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCapsReinforcementComponent();

	// Blueprint subclass of ACapsEnemyBase spawned as reinforcements.
	// Assign in the dungeon generator's Details panel.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Reinforcement")
	TSubclassOf<ACapsEnemyBase> ReinforcementEnemyClass;

	// Fired when a wave is about to spawn — use in Blueprint to show a warning UI.
	UPROPERTY(BlueprintAssignable, Category="Reinforcement")
	FOnWaveIncoming OnWaveIncoming;

	// Call when a room clears. Registers it as a new wave source.
	void NotifyRoomCleared(int32 RoomIndex);

	// Clears all sources. Call at the start of each run.
	void ResetForNewRun();

protected:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

private:
	TArray<FReinforcementSource> ActiveSources;

	void SpawnWave(FReinforcementSource& Source);

	UFUNCTION()
	void OnReinforcementEnemyDied(ACapsEnemyBase* Enemy);

	ACapsDungeonGenerator* GetGenerator() const;
};
