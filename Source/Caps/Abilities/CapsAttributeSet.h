#pragma once

#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "CapsAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
	GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class CAPS_API UCapsAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UCapsAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// Current health. Clamped to [0, MaxHealth].
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Health", ReplicatedUsing=OnRep_Health)
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UCapsAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category="Attributes|Health", ReplicatedUsing=OnRep_MaxHealth)
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UCapsAttributeSet, MaxHealth)

	// Base attack damage — modified by meal ingredients in the Attack slots.
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat", ReplicatedUsing=OnRep_AttackDamage)
	FGameplayAttributeData AttackDamage;
	ATTRIBUTE_ACCESSORS(UCapsAttributeSet, AttackDamage)

	// Character move speed — modified by meal ingredients in the Movement slot.
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Movement", ReplicatedUsing=OnRep_MoveSpeed)
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UCapsAttributeSet, MoveSpeed)

	// Projectiles fired per attack activation. Food / character traits can raise this
	// (e.g. Triple Shot sets it to 3). Weapon abilities read this and spawn N projectiles.
	// Default 1. Clamped to [1, ∞].
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat")
	FGameplayAttributeData ProjectileCount;
	ATTRIBUTE_ACCESSORS(UCapsAttributeSet, ProjectileCount)

	// Multiplier applied to hit-sphere radius and projectile scale.
	// Default 1.0. Food can increase this to make attacks cover more area.
	// Clamped to [0.1, ∞].
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Combat")
	FGameplayAttributeData AttackRadius;
	ATTRIBUTE_ACCESSORS(UCapsAttributeSet, AttackRadius)

	// Number of potion uses remaining for this run. Set by a GE applied at run start
	// (driven by the Drink dish configuration). The DrinkPotion ability checks this > 0
	// before activating and decrements it via an Instant GE on success.
	// Default 0. Clamped to [0, ∞].
	UPROPERTY(BlueprintReadOnly, Category="Attributes|Potion")
	FGameplayAttributeData PotionCharges;
	ATTRIBUTE_ACCESSORS(UCapsAttributeSet, PotionCharges)

protected:
	UFUNCTION()
	void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UFUNCTION()
	void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	UFUNCTION()
	void OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage);

	UFUNCTION()
	void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
};
