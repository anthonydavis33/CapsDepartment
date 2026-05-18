#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CapsExtractionTriggerActor.generated.h"

class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPlayerExtracted);

// Place inside a Safe room Level Instance. When the player overlaps it,
// HandleExtraction() is called on their inventory and OnPlayerExtracted fires.
// Bind OnPlayerExtracted in Blueprint to trigger the hub level transition.
//
// Future: add vendor/gambling actors alongside this in the same Level Instance.
UCLASS()
class CAPS_API ACapsExtractionTriggerActor : public AActor
{
	GENERATED_BODY()

public:
	ACapsExtractionTriggerActor();

	// Fired after extraction is processed. Bind in Blueprint to load the hub level.
	UPROPERTY(BlueprintAssignable, Category="Extraction")
	FOnPlayerExtracted OnPlayerExtracted;

protected:
	virtual void BeginPlay() override;

	// Override in Blueprint for VFX, sound, or UI fade before the level transition.
	UFUNCTION(BlueprintImplementableEvent, Category="Extraction")
	void OnExtractionActivated();

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> TriggerVolume;

	bool bAlreadyTriggered = false;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex,
		bool bFromSweep, const FHitResult& SweepResult);
};
