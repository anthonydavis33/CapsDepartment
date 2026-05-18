#include "Boss/CapsBossBase.h"
#include "Boss/CapsBossModifierDataAsset.h"
#include "Boss/CapsBossDoorActor.h"
#include "Abilities/CapsAbilitySystemComponent.h"
#include "Abilities/CapsAttributeSet.h"
#include "Kismet/GameplayStatics.h"

ACapsBossBase::ACapsBossBase()
{
	// Boss tick is off by default (same as base enemy).
	// Enable in Blueprint if needed for phase-specific behavior.
}

void ACapsBossBase::BeginPlay()
{
	Super::BeginPlay(); // enemy base binds death detection

	// Bind a second listener for phase transitions (does not interfere with base death logic).
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UCapsAttributeSet::GetHealthAttribute())
			.AddUObject(this, &ACapsBossBase::OnBossHealthChanged);
	}

	// Auto-detect the boss door and grab this run's modifiers.
	if (ACapsBossDoorActor* Door = Cast<ACapsBossDoorActor>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ACapsBossDoorActor::StaticClass())))
	{
		TArray<UCapsBossModifierDataAsset*> Mods;
		for (const TObjectPtr<UCapsBossModifierDataAsset>& Mod : Door->ActiveModifiers)
		{
			if (Mod) Mods.Add(Mod.Get());
		}
		ActivateModifiers(Mods);
	}
}

void ACapsBossBase::ActivateModifiers(const TArray<UCapsBossModifierDataAsset*>& Modifiers)
{
	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();

	for (UCapsBossModifierDataAsset* Mod : Modifiers)
	{
		if (!Mod) continue;
		ActiveModifiers.Add(Mod);

		if (Mod->BossEffect && ASC)
		{
			FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(Mod->BossEffect, 1.f, Context);
			ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
		}
	}
}

void ACapsBossBase::OnBossHealthChanged(const FOnAttributeChangeData& Data)
{
	// Let the base class handle death (NewValue <= 0). Only check phase here.
	if (bPhase2Triggered || Data.NewValue <= 0.f) return;

	const UCapsAttributeSet* Attrs = GetAttributeSet();
	if (!Attrs) return;

	const float MaxHealth = Attrs->GetMaxHealth();
	if (MaxHealth > 0.f && Data.NewValue <= MaxHealth * Phase2HealthThreshold)
	{
		bPhase2Triggered = true;
		CurrentPhase = 1;
		BP_OnPhaseChanged(CurrentPhase);
	}
}
