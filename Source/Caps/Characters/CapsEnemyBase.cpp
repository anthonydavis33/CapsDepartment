#include "Characters/CapsEnemyBase.h"
#include "Abilities/CapsAbilitySystemComponent.h"
#include "Abilities/CapsAttributeSet.h"
#include "Items/CapsLootTableDataAsset.h"
#include "GameFramework/CharacterMovementComponent.h"

ACapsEnemyBase::ACapsEnemyBase()
{
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UCapsAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(false);

	AttributeSet = CreateDefaultSubobject<UCapsAttributeSet>(TEXT("AttributeSet"));
}

void ACapsEnemyBase::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->InitAbilityActorInfo(this, this);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		UCapsAttributeSet::GetHealthAttribute()
	).AddUObject(this, &ACapsEnemyBase::OnHealthChanged);
}

UAbilitySystemComponent* ACapsEnemyBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent.Get();
}

void ACapsEnemyBase::ApplyScaling(const FDungeonScalingTier& Tier)
{
	if (!AbilitySystemComponent) return;

	auto ScaleAttr = [&](FGameplayAttribute Attr, float Multiplier)
	{
		const float Base = AbilitySystemComponent->GetNumericAttributeBase(Attr);
		AbilitySystemComponent->SetNumericAttributeBase(Attr, Base * Multiplier);
	};

	ScaleAttr(UCapsAttributeSet::GetHealthAttribute(),      Tier.HealthMultiplier);
	ScaleAttr(UCapsAttributeSet::GetMaxHealthAttribute(),   Tier.HealthMultiplier);
	ScaleAttr(UCapsAttributeSet::GetAttackDamageAttribute(), Tier.DamageMultiplier);
	ScaleAttr(UCapsAttributeSet::GetMoveSpeedAttribute(),   Tier.SpeedMultiplier);

	// Sync movement component to scaled speed attribute
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
	{
		Movement->MaxWalkSpeed = AbilitySystemComponent->GetNumericAttribute(
			UCapsAttributeSet::GetMoveSpeedAttribute());
	}

	// Apply any milestone GAS effects for this tier
	for (TSubclassOf<UGameplayEffect> EffectClass : Tier.MilestoneBuffEffects)
	{
		if (!EffectClass) continue;
		FGameplayEffectContextHandle Context = AbilitySystemComponent->MakeEffectContext();
		FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(EffectClass, 1.f, Context);
		AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}

void ACapsEnemyBase::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	if (Data.NewValue <= 0.f && !bIsDead)
	{
		bIsDead = true;
		HandleDeath();
	}
}

void ACapsEnemyBase::HandleDeath()
{
	// Roll loot and notify Blueprint to spawn pickups
	if (LootTable)
	{
		TArray<FName> Drops = LootTable->RollDrops();
		if (!Drops.IsEmpty())
			BP_OnDropLoot(Drops, GetActorLocation());
	}

	OnEnemyDied.Broadcast(this);
	BP_OnDeath();

	// Disable collision and movement so the corpse doesn't block anything
	SetActorEnableCollision(false);
	if (UCharacterMovementComponent* Movement = GetCharacterMovement())
		Movement->DisableMovement();

	// Give BP_OnDeath time to play before destroying
	SetLifeSpan(3.f);
}
