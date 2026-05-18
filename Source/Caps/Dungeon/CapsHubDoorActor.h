#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CapsHubDoorActor.generated.h"

class UBoxComponent;
class ACapsGameMode;

// The door in the hub that starts a dungeon run.
// When the player enters the inner trigger volume, the transition begins:
//   1. Input is disabled (cinematic mode).
//   2. BP_OnPlayerEntered fires — play your walk-through cinematic here.
//   3. When the cinematic is ready, call CommitTransition() (from BP or a timer).
//   4. CommitTransition() calls GameMode->BeginDungeonTransition() and seals the door.
//
// The door is ALSO visible on the dungeon minimap as the Hub room entry (index -1),
// shown as sealed with the Hub ERoomType so the minimap widget can draw chains.
//
// Blueprint guide:
//   - Override BP_OnPlayerEntered for the walk-through cinematic (disable input,
//     camera dolly, play hub-door-sealing animation).
//   - Call CommitTransition() from a Timeline / AnimNotify when the cinematic ends.
//   - Override BP_OnSealed for persistent "chains" visual once the run starts.
UCLASS(Blueprintable)
class CAPS_API ACapsHubDoorActor : public AActor
{
	GENERATED_BODY()

public:
	ACapsHubDoorActor();

	// Call from Blueprint at the END of the entry cinematic to load the dungeon.
	UFUNCTION(BlueprintCallable, Category="HubDoor")
	void CommitTransition();

	// Seals the door visually (called automatically after CommitTransition).
	UFUNCTION(BlueprintCallable, Category="HubDoor")
	void SealDoor();

	bool IsSealed() const { return bIsSealed; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="HubDoor")
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	// Chains / seal visual. Hidden by default, shown when bIsSealed = true.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="HubDoor")
	TObjectPtr<UStaticMeshComponent> SealMesh;

	// Player walks into this to trigger the entry sequence.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="HubDoor")
	TObjectPtr<UBoxComponent> EntryVolume;

	// Play entry cinematic here. Call CommitTransition() when finished.
	UFUNCTION(BlueprintImplementableEvent, Category="HubDoor")
	void BP_OnPlayerEntered();

	// Apply chains / sealed visual here. Called once after CommitTransition.
	UFUNCTION(BlueprintImplementableEvent, Category="HubDoor")
	void BP_OnSealed();

private:
	bool bIsSealed       = false;
	bool bTransitionFired = false;

	UFUNCTION()
	void OnEntryOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
