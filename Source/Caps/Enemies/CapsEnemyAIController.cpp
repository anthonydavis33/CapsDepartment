#include "Enemies/CapsEnemyAIController.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

ACapsEnemyAIController::ACapsEnemyAIController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ACapsEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	TimeSinceLastAttack = AttackCooldown; // ready to attack immediately on first contact
}

void ACapsEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APawn* ControlledPawn = GetPawn();
	if (!ControlledPawn) return;

	ACharacter* Player = UGameplayStatics::GetPlayerCharacter(this, 0);
	if (!Player) return;

	TimeSinceLastAttack += DeltaTime;

	const float DistSq = FVector::DistSquared(
		ControlledPawn->GetActorLocation(),
		Player->GetActorLocation()
	);
	const float AggroRangeSq  = AggroRange  * AggroRange;
	const float AttackRangeSq = AttackRange * AttackRange;

	if (DistSq <= AttackRangeSq)
	{
		// In attack range — stop moving and attack on cooldown
		StopMovement();
		bIsAggroed = true;

		if (TimeSinceLastAttack >= AttackCooldown)
		{
			TimeSinceLastAttack = 0.f;
			TriggerAttack();
		}
	}
	else if (DistSq <= AggroRangeSq)
	{
		// In aggro range — chase the player
		bIsAggroed = true;
		MoveToActor(Player, AttackRange * 0.9f);
	}
	else if (bIsAggroed)
	{
		// Player escaped aggro range — return to idle
		bIsAggroed = false;
		StopMovement();
	}
}
