#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Dungeon/CapsRoomTypes.h"
#include "CapsDoorActor.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UCapsTilesetDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerPassedThrough, ECardinalDirection, Direction);

// Placed at the edge of a room template. The generator seals unused doors
// and connects open ones to the matching door of the adjacent room.
UCLASS()
class CAPS_API ACapsDoorActor : public AActor
{
	GENERATED_BODY()

public:
	ACapsDoorActor();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door")
	ECardinalDirection Direction = ECardinalDirection::North;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Door")
	EDoorState State = EDoorState::Sealed;

	// Fired when the player walks through this door while Open.
	UPROPERTY(BlueprintAssignable, Category="Door")
	FOnPlayerPassedThrough OnPlayerPassedThrough;

	UFUNCTION(BlueprintCallable, Category="Door")
	void SetState(EDoorState NewState);

	// Implement in Blueprint to drive animations, VFX, or audio on state change.
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnStateChanged(EDoorState NewState);

	// Swaps the door mesh to match the given tileset and fires OnTilesetApplied.
	UFUNCTION(BlueprintCallable, Category="Door")
	void ApplyTileset(UCapsTilesetDataAsset* Tileset);

	// Implement in Blueprint for theme-specific door animations or effects.
	UFUNCTION(BlueprintImplementableEvent, Category="Door")
	void OnTilesetApplied(UCapsTilesetDataAsset* Tileset);

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> TriggerVolume;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> DoorMesh;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	void UpdateVisuals();
};
