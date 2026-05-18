#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayEffect.h"
#include "CapsCharacterDataAsset.generated.h"

class UCapsGameplayAbility;

// One asset per playable character. Assign in Project Settings → Caps Dept → Available Characters.
//
// Designer workflow:
//   1. Create a new UCapsCharacterDataAsset asset.
//   2. Fill in display info (name, portrait, pros/cons text).
//   3. Assign Mesh + AnimClass (the character's skeletal mesh and ABP).
//   4. Create a GE_<CharacterName>_Stats GameplayEffect asset and assign it to
//      CharacterStatEffect. Use Infinite duration + Multiply/Override modifiers
//      to express the character's pros/cons (e.g. 0.7× MoveSpeed, 1.5× AttackDamage).
//   5. Assign the character's unique main weapon ability (GA_<Name>_MainWeapon).
//   6. Assign a default secondary weapon (swappable in-run via pickup actors).
//   7. Add any passive abilities (e.g. GA_<Name>_Passive_AutoFire).
//   8. Add the asset to UCapsGameSettings::AvailableCharacters.
UCLASS(BlueprintType)
class CAPS_API UCapsCharacterDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	// ── Display ───────────────────────────────────────────────────────────────

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
	FText CharacterName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
	TSoftObjectPtr<UTexture2D> Portrait;

	// Shown as green bullet points on the character card.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
	TArray<FText> Pros;

	// Shown as red bullet points on the character card.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
	TArray<FText> Cons;

	// ── Appearance ────────────────────────────────────────────────────────────

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Appearance")
	TSoftObjectPtr<USkeletalMesh> Mesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Appearance")
	TSubclassOf<UAnimInstance> AnimClass;

	// ── Stats ─────────────────────────────────────────────────────────────────

	// Applied as an Infinite-duration GAS GameplayEffect when the run starts.
	// Use Multiply / Override modifiers to express the character's stat profile.
	// e.g. MoveSpeed × 0.7, AttackDamage × 1.5, ProjectileCount Override 1.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Stats")
	TSubclassOf<UGameplayEffect> CharacterStatEffect;

	// ── Abilities ─────────────────────────────────────────────────────────────

	// This character's UNIQUE primary weapon. Granted at run start and never swappable.
	// InputID on the ability must be set to Attack1.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abilities")
	TSubclassOf<UCapsGameplayAbility> MainWeaponAbility;

	// The secondary weapon the player starts with. Can be swapped mid-run via pickups.
	// InputID on the ability must be set to Attack2.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abilities")
	TSubclassOf<UCapsGameplayAbility> DefaultSecondaryAbility;

	// Character-specific passives (auto-fire trait, dash-strike on dodge, etc.).
	// Granted at run start; InputID = None for passives (activated by tags/events).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Abilities")
	TArray<TSubclassOf<UCapsGameplayAbility>> PassiveAbilities;
};
