#include "Core/CapsGameMode.h"
#include "Core/CapsGameInstance.h"
#include "Core/CapsGameSettings.h"
#include "Characters/CapsCharacterDataAsset.h"
#include "Characters/CapsSecondaryWeaponComponent.h"
#include "Abilities/CapsAbilitySystemComponent.h"
#include "Dungeon/CapsDungeonGenerator.h"
#include "Items/CapsInventoryComponent.h"
#include "CapsCharacter.h"
#include "Kismet/GameplayStatics.h"

ACapsGameMode::ACapsGameMode()
{
}

void ACapsGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (bIsDungeonLevel())
	{
		ApplySelectedCharacterToPlayer();
		StartDungeonRun();
		BP_OnDungeonReady();
	}
	else
	{
		// Hub: restore the player's ingredient stock from the last save.
		if (UCapsGameInstance* GI = UCapsGameInstance::Get(this))
		{
			APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
			ACapsCharacter* Character = PC ? Cast<ACapsCharacter>(PC->GetPawn()) : nullptr;
			if (Character)
				GI->RestoreInventoryFromSave(Character->GetInventoryComponent());
		}
		BP_OnHubReady();
	}
}

void ACapsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	// Character application is handled in BeginPlay after the world is ready.
}

void ACapsGameMode::BeginDungeonTransition()
{
	if (UCapsGameInstance* GI = UCapsGameInstance::Get(this))
		GI->SetRunState(ECapsRunState::Transitioning);

	BP_OnTransitionToDungeon();

	const FName DungeonLevel = UCapsGameSettings::Get()->DungeonLevelName;
	UGameplayStatics::OpenLevel(this, DungeonLevel);
}

void ACapsGameMode::ReturnToHub()
{
	if (UCapsGameInstance* GI = UCapsGameInstance::Get(this))
	{
		GI->NotifyRunEnded();

		// Save the player's current inventory before the level unloads.
		// Grabs the pawn here because it's still alive at this call site
		// (extraction trigger or death handler calls us before any transition).
		APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
		ACapsCharacter* Character = PC ? Cast<ACapsCharacter>(PC->GetPawn()) : nullptr;
		GI->SaveCurrentRun(Character ? Character->GetInventoryComponent() : nullptr);
	}

	BP_OnTransitionToHub();

	const FName HubLevel = UCapsGameSettings::Get()->HubLevelName;
	UGameplayStatics::OpenLevel(this, HubLevel);
}

void ACapsGameMode::ApplySelectedCharacterToPlayer()
{
	UCapsGameInstance* GI = UCapsGameInstance::Get(this);
	if (!GI || !GI->SelectedCharacter) return;

	APlayerController* PC = UGameplayStatics::GetPlayerController(this, 0);
	ACapsCharacter* Character = PC ? Cast<ACapsCharacter>(PC->GetPawn()) : nullptr;
	if (!Character) return;

	ApplyCharacterData(Character, GI->SelectedCharacter);
	GI->SetRunState(ECapsRunState::InRun);
}

void ACapsGameMode::ApplyCharacterData(ACapsCharacter* Character, UCapsCharacterDataAsset* CharData)
{
	if (!Character || !CharData) return;

	UCapsAbilitySystemComponent* ASC =
		Cast<UCapsAbilitySystemComponent>(Character->GetAbilitySystemComponent());
	if (!ASC) return;

	// Apply the character's stat modifier GE (speed, damage multipliers, etc.)
	if (CharData->CharacterStatEffect)
	{
		FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
		FGameplayEffectSpecHandle Spec =
			ASC->MakeOutgoingSpec(CharData->CharacterStatEffect, 1.f, Context);
		ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
	}

	// Grant the unique main weapon ability.
	if (CharData->MainWeaponAbility)
		ASC->GrantAbility(CharData->MainWeaponAbility);

	// Grant any character-specific passives.
	for (const TSubclassOf<UCapsGameplayAbility>& Passive : CharData->PassiveAbilities)
		ASC->GrantAbility(Passive);

	// Equip the default secondary weapon via the dedicated component.
	if (UCapsSecondaryWeaponComponent* SecComp =
		Character->FindComponentByClass<UCapsSecondaryWeaponComponent>())
	{
		SecComp->EquipSecondary(CharData->DefaultSecondaryAbility);
	}

	// Swap the skeletal mesh (sync load — acceptable at run start).
	if (!CharData->Mesh.IsNull())
	{
		if (USkeletalMesh* SkelMesh = CharData->Mesh.LoadSynchronous())
			Character->GetMesh()->SetSkeletalMesh(SkelMesh);
	}
	if (CharData->AnimClass)
		Character->GetMesh()->SetAnimInstanceClass(CharData->AnimClass);
}

bool ACapsGameMode::bIsDungeonLevel() const
{
	const FName CurrentLevel = FName(*UGameplayStatics::GetCurrentLevelName(this, true));
	return CurrentLevel == UCapsGameSettings::Get()->DungeonLevelName;
}

void ACapsGameMode::StartDungeonRun()
{
	ACapsDungeonGenerator* Generator = Cast<ACapsDungeonGenerator>(
		UGameplayStatics::GetActorOfClass(this, ACapsDungeonGenerator::StaticClass()));
	if (Generator)
		Generator->StartRun();
}
