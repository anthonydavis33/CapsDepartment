#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "CapsEnemyAIController.generated.h"

// Simple distance-based AI: idle → aggro → chase → attack → repeat.
// No behavior tree assets required — all logic runs in Tick.
// Subclass in Blueprint to implement TriggerAttack for per-enemy attack behavior.
UCLASS()
class CAPS_API ACapsEnemyAIController : public AAIController
{
	GENERATED_BODY()

public:
	ACapsEnemyAIController();

	// Distance at which the enemy notices and begins chasing the player.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float AggroRange = 800.f;

	// Distance at which the enemy stops and attacks.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float AttackRange = 150.f;

	// Minimum seconds between attack triggers.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="AI")
	float AttackCooldown = 1.5f;

protected:
	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	// Implement in Blueprint to activate the enemy's attack ability or play an attack montage.
	UFUNCTION(BlueprintImplementableEvent, Category="AI")
	void TriggerAttack();

private:
	float TimeSinceLastAttack = 0.f;
	bool bIsAggroed = false;
};
