#include "Boss/CapsBossDoorActor.h"
#include "Boss/CapsBossModifierDataAsset.h"
#include "Boss/CapsBossModifierPoolDataAsset.h"
#include "UI/CapsModifierRevealWidget.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "Components/BoxComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"

ACapsBossDoorActor::ACapsBossDoorActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);

	ProximityVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("ProximityVolume"));
	ProximityVolume->SetupAttachment(RootComponent);
	ProximityVolume->SetBoxExtent(FVector(300.f, 300.f, 150.f));
	ProximityVolume->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
}

void ACapsBossDoorActor::BeginPlay()
{
	Super::BeginPlay();
	ProximityVolume->OnComponentBeginOverlap.AddDynamic(this, &ACapsBossDoorActor::OnProximityBeginOverlap);

	// Pre-roll modifiers at BeginPlay so ActiveModifiers is stable before the player arrives.
	// ACapsBossBase reads this in its own BeginPlay.
	RollModifiers();
}

void ACapsBossDoorActor::RollModifiers()
{
	if (bModifiersRolled || !ModifierPool) return;
	bModifiersRolled = true;

	for (UCapsBossModifierDataAsset* Mod : ModifierPool->DrawModifiers(ModifiersToReveal))
	{
		if (Mod) ActiveModifiers.Add(Mod);
	}
}

void ACapsBossDoorActor::OnProximityBeginOverlap(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor,
	UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*SweepResult*/)
{
	if (ActiveRevealWidget) return; // widget already open

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	if (!PC || PC->GetPawn() != OtherActor) return;

	ShowRevealWidget(PC);
}

void ACapsBossDoorActor::ShowRevealWidget(APlayerController* PC)
{
	if (!RevealWidgetClass) return;

	ActiveRevealWidget = CreateWidget<UCapsModifierRevealWidget>(PC, RevealWidgetClass);
	if (!ActiveRevealWidget) return;

	TArray<UCapsBossModifierDataAsset*> ModPtrs;
	for (const TObjectPtr<UCapsBossModifierDataAsset>& Mod : ActiveModifiers)
	{
		if (Mod) ModPtrs.Add(Mod.Get());
	}

	ActiveRevealWidget->InitializeModifiers(ModPtrs, this);
	ActiveRevealWidget->AddToViewport();

	// Give focus to UI so the player can read and choose without the pawn moving.
	PC->SetInputMode(FInputModeUIOnly());
	PC->SetShowMouseCursor(true);
}

void ACapsBossDoorActor::OnPlayerCommittedEntry()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (ActiveRevealWidget)
	{
		ActiveRevealWidget->RemoveFromParent();
		ActiveRevealWidget = nullptr;
	}

	if (PC)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);

		if (APawn* Player = PC->GetPawn())
			ApplyPlayerModifierEffects(Player);
	}

	BP_OnPlayerEntered();
	OnPlayerEnteredBossRoom.Broadcast();
}

void ACapsBossDoorActor::OnPlayerRetreated()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);

	if (ActiveRevealWidget)
	{
		ActiveRevealWidget->RemoveFromParent();
		ActiveRevealWidget = nullptr;
	}

	if (PC)
	{
		PC->SetInputMode(FInputModeGameOnly());
		PC->SetShowMouseCursor(false);
	}

	BP_OnPlayerRetreated();
}

void ACapsBossDoorActor::ApplyPlayerModifierEffects(APawn* Player)
{
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Player);
	UAbilitySystemComponent* ASC = ASI ? ASI->GetAbilitySystemComponent() : nullptr;
	if (!ASC) return;

	for (const TObjectPtr<UCapsBossModifierDataAsset>& Mod : ActiveModifiers)
	{
		if (!Mod || !Mod->PlayerEffect) continue;
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(Mod->PlayerEffect, 1.f, Context);
		ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}
}
