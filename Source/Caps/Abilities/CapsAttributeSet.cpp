#include "CapsAttributeSet.h"
#include "Net/UnrealNetwork.h"
#include "GameplayEffectExtension.h"

UCapsAttributeSet::UCapsAttributeSet()
{
	InitHealth(100.f);
	InitMaxHealth(100.f);
	InitAttackDamage(10.f);
	InitMoveSpeed(600.f);
	InitProjectileCount(1.f);
	InitAttackRadius(1.f);
	InitPotionCharges(0.f);
}

void UCapsAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME_CONDITION_NOTIFY(UCapsAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCapsAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCapsAttributeSet, AttackDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UCapsAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
}

void UCapsAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if (Attribute == GetHealthAttribute())
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	else if (Attribute == GetProjectileCountAttribute())
		NewValue = FMath::Max(NewValue, 1.f);
	else if (Attribute == GetAttackRadiusAttribute())
		NewValue = FMath::Max(NewValue, 0.1f);
	else if (Attribute == GetPotionChargesAttribute())
		NewValue = FMath::Max(NewValue, 0.f);
}

void UCapsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
}

void UCapsAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCapsAttributeSet, Health, OldHealth);
}

void UCapsAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCapsAttributeSet, MaxHealth, OldMaxHealth);
}

void UCapsAttributeSet::OnRep_AttackDamage(const FGameplayAttributeData& OldAttackDamage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCapsAttributeSet, AttackDamage, OldAttackDamage);
}

void UCapsAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UCapsAttributeSet, MoveSpeed, OldMoveSpeed);
}
