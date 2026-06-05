#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Cooking/CookingTypes.h"
#include "CapsIngredientPickupActor.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class ACapsCharacter;

// Spawned in the world when an enemy drops loot (via BP_OnDropLoot on CapsEnemyBase).
// The player walks over it to add the ingredient to their run pickups.
//
// Blueprint workflow:
//   1. Subclass this in BP (e.g. BP_IngredientPickup).
//   2. Set a StaticMesh or Niagara component on the root for visual representation.
//   3. Override BP_OnPickedUp to play a collect VFX/sound.
//   4. In BP_OnDropLoot on your enemy Blueprint, spawn this actor, set IngredientID.
//
// The pickup destroys itself after granting the ingredient — no manual cleanup needed.
UCLASS(Blueprintable)
class CAPS_API ACapsIngredientPickupActor : public AActor
{
	GENERATED_BODY()

public:
	ACapsIngredientPickupActor();

	// ── Data ─────────────────────────────────────────────────────────────────

	// The ingredient this pickup grants. Must match a valid UCapsIngredientDataAsset
	// PrimaryAssetId. Set this when spawning from BP_OnDropLoot.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup")
	FName IngredientID;

	// Quality tier of this pickup. Set by the loot table when spawning.
	// Drives GE level when the ingredient is slotted and eaten.
	// Also used by BP_IngredientPickup to set visual differentiation (color, glow, particles).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup")
	EIngredientQuality Quality = EIngredientQuality::Choice;

	// Number of units granted on pickup. Rolled from FLootEntry::MinCount/MaxCount
	// before spawning; just stored here for display and transfer.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Pickup", meta=(ClampMin="1"))
	int32 Quantity = 1;

protected:
	virtual void BeginPlay() override;

	// Override to play a collect particle, sound, or UI pop.
	// Called immediately before the actor is destroyed.
	UFUNCTION(BlueprintImplementableEvent, Category="Pickup")
	void BP_OnPickedUp(ACapsCharacter* Picker);

private:
	// Pawn-only overlap sphere. Radius tunable in Blueprint via component properties.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Pickup", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USphereComponent> CollisionSphere;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
