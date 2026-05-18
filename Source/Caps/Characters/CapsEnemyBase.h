#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffectTypes.h"
#include "Dungeon/CapsRoomTypes.h"
#include "CapsEnemyBase.generated.h"

class UCapsAbilitySystemComponent;
class UCapsAttributeSet;
class UCapsLootTableDataAsset;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEnemyDied, ACapsEnemyBase*, Enemy);

// Base class for all dungeon enemies. Subclass in Blueprint to set mesh,
// animations, loot table, and implement BP_OnDeath / BP_OnDropLoot.
// The spawning room template listens to OnEnemyDied to track room-clear state.
UCLASS(abstract)
class CAPS_API ACapsEnemyBase : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ACapsEnemyBase();

	// IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UCapsAttributeSet* GetAttributeSet() const { return AttributeSet.Get(); }

	// Scales Health, AttackDamage, and MoveSpeed by the active dungeon tier.
	// Call this immediately after spawning the enemy.
	UFUNCTION(BlueprintCallable, Category="Enemy")
	void ApplyScaling(const FDungeonScalingTier& Tier);

	// Broadcast when this enemy dies, before the actor is destroyed.
	// Bind from the room template Blueprint to track room-clear state.
	UPROPERTY(BlueprintAssignable, Category="Enemy")
	FOnEnemyDied OnEnemyDied;

	bool IsAlive() const { return !bIsDead; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Enemy")
	TObjectPtr<UCapsLootTableDataAsset> LootTable;

	// Override in Blueprint to play death animation, ragdoll, VFX, etc.
	UFUNCTION(BlueprintImplementableEvent, Category="Enemy")
	void BP_OnDeath();

	// Override in Blueprint to spawn pickup actors at DropLocation.
	// IngredientIDs are the rolled loot — one entry per unit to drop.
	UFUNCTION(BlueprintImplementableEvent, Category="Enemy")
	void BP_OnDropLoot(const TArray<FName>& IngredientIDs, FVector DropLocation);

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Abilities", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCapsAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UCapsAttributeSet> AttributeSet;

	bool bIsDead = false;

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void HandleDeath();
};
