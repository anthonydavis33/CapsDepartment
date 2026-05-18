#include "Extraction/CapsExtractionTriggerActor.h"
#include "Items/CapsInventoryComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"

ACapsExtractionTriggerActor::ACapsExtractionTriggerActor()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	TriggerVolume = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerVolume"));
	TriggerVolume->SetupAttachment(RootComponent);
	TriggerVolume->SetBoxExtent(FVector(120.f, 120.f, 96.f));
	TriggerVolume->SetCollisionProfileName(TEXT("OverlapOnlyPawn"));
}

void ACapsExtractionTriggerActor::BeginPlay()
{
	Super::BeginPlay();
	TriggerVolume->OnComponentBeginOverlap.AddDynamic(this, &ACapsExtractionTriggerActor::OnOverlapBegin);
}

void ACapsExtractionTriggerActor::OnOverlapBegin(UPrimitiveComponent*, AActor* OtherActor,
	UPrimitiveComponent*, int32, bool, const FHitResult&)
{
	if (bAlreadyTriggered) return;

	ACharacter* Character = Cast<ACharacter>(OtherActor);
	if (!Character) return;

	UCapsInventoryComponent* Inventory = Character->FindComponentByClass<UCapsInventoryComponent>();
	if (!Inventory) return;

	bAlreadyTriggered = true;

	Inventory->HandleExtraction();
	OnExtractionActivated();
	OnPlayerExtracted.Broadcast();
}
