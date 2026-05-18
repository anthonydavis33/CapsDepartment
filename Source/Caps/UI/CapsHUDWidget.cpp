#include "UI/CapsHUDWidget.h"
#include "Dungeon/CapsDungeonGenerator.h"
#include "Enemies/CapsReinforcementComponent.h"
#include "Abilities/CapsAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "CapsCharacter.h"
#include "Kismet/GameplayStatics.h"

void UCapsHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	CachedGenerator = Cast<ACapsDungeonGenerator>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ACapsDungeonGenerator::StaticClass()));

	BindHealthDelegate();
	BindReinforcementDelegate();

	// Fire an initial room sync so the display starts correct.
	PollDungeonState();
}

void UCapsHUDWidget::NativeDestruct()
{
	Super::NativeDestruct();

	// Unbind health delegate so we don't get callbacks on a dead widget.
	if (APlayerController* PC = GetOwningPlayer())
	{
		if (IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PC->GetPawn()))
		{
			if (UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent())
			{
				ASC->GetGameplayAttributeValueChangeDelegate(
					UCapsAttributeSet::GetHealthAttribute())
					.RemoveAll(this);
			}
		}
	}
}

void UCapsHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	RefreshAccumulator += InDeltaTime;
	if (RefreshAccumulator < RefreshInterval) return;
	RefreshAccumulator = 0.f;

	PollDungeonState();
}

void UCapsHUDWidget::BindHealthDelegate()
{
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PC->GetPawn());
	UAbilitySystemComponent* ASC = ASI ? ASI->GetAbilitySystemComponent() : nullptr;
	if (!ASC) return;

	ASC->GetGameplayAttributeValueChangeDelegate(UCapsAttributeSet::GetHealthAttribute())
		.AddUObject(this, &UCapsHUDWidget::OnHealthAttributeChanged);

	// Fire once immediately so the bar starts filled.
	const float Current = ASC->GetNumericAttribute(UCapsAttributeSet::GetHealthAttribute());
	const float Max     = ASC->GetNumericAttribute(UCapsAttributeSet::GetMaxHealthAttribute());
	OnHealthChanged(Current, Max);
}

void UCapsHUDWidget::BindReinforcementDelegate()
{
	if (!CachedGenerator) return;

	// ReinforcementComponent is private on the generator — access via the component getter.
	if (UCapsReinforcementComponent* RC = CachedGenerator->FindComponentByClass<UCapsReinforcementComponent>())
	{
		RC->OnWaveIncoming.AddDynamic(this, &UCapsHUDWidget::HandleWaveIncoming);
	}
}

void UCapsHUDWidget::OnHealthAttributeChanged(const FOnAttributeChangeData& Data)
{
	// Derive MaxHealth from the ASC so we can pass both to Blueprint.
	APlayerController* PC = GetOwningPlayer();
	if (!PC) return;

	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(PC->GetPawn());
	UAbilitySystemComponent* ASC = ASI ? ASI->GetAbilitySystemComponent() : nullptr;
	if (!ASC) return;

	const float Max = ASC->GetNumericAttribute(UCapsAttributeSet::GetMaxHealthAttribute());
	OnHealthChanged(Data.NewValue, Max);
}

void UCapsHUDWidget::HandleWaveIncoming()
{
	OnReinforcementWarning();
}

void UCapsHUDWidget::PollDungeonState()
{
	if (!CachedGenerator) return;

	// GetCurrentRoomTemplate is public — infer room index from it via minimap data
	// by checking the entry where bIsCurrent == true.
	const TArray<FMinimapRoomEntry> MinimapData = CachedGenerator->GetMinimapData();
	for (const FMinimapRoomEntry& Entry : MinimapData)
	{
		if (Entry.bIsCurrent && Entry.RoomIndex != LastRoomIndex)
		{
			LastRoomIndex = Entry.RoomIndex;
			OnRoomChanged(LastRoomIndex);
			break;
		}
	}
}
