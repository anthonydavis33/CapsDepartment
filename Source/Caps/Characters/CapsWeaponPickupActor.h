#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CapsWeaponPickupActor.generated.h"

class UBoxComponent;
class UCapsGameplayAbility;

// Dropped weapon the player can pick up mid-run to swap their secondary.
// Place in a room template or spawn dynamically from a loot table.
//
// On player overlap:
//   1. Gives the player's UCapsSecondaryWeaponComponent the new weapon class.
//   2. Spawns a new CapsWeaponPickupActor for the REPLACED weapon (drop-swap).
//   3. Destroys itself.
//
// Blueprint guide:
//   - Set WeaponClass in the Details panel.
//   - Override BP_OnPickedUp to play VFX / sound before the actor destroys itself.
//   - The SpawnedPickupClass must also be a UCapsWeaponPickupActor subclass —
//     set it to the same class as this actor unless you want a different look.
UCLASS(Blueprintable)
class CAPS_API ACapsWeaponPickupActor : public AActor
{
	GENERATED_BODY()

public:
	ACapsWeaponPickupActor();

	// The weapon ability granted to the player on pickup.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	TSubclassOf<UCapsGameplayAbility> WeaponClass;

	// Class spawned for the weapon the player drops when they pick this one up.
	// Defaults to this actor's class. Override for a different visual per weapon type.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Weapon")
	TSubclassOf<ACapsWeaponPickupActor> SpawnedPickupClass;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Weapon")
	TObjectPtr<UBoxComponent> OverlapVolume;

	// Play pickup VFX / sound here. Actor destroys itself immediately after.
	UFUNCTION(BlueprintImplementableEvent, Category="Weapon")
	void BP_OnPickedUp(ACharacter* Picker);

private:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);

	bool bAlreadyPickedUp = false;
};
