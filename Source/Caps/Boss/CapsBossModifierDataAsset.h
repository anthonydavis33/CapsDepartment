#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayEffect.h"
#include "CapsBossModifierDataAsset.generated.h"

// Which category of effect this modifier represents.
// Used by Blueprint UI to color-code modifier cards and by room templates
// to decide whether to activate environmental / enemy-spawn behavior.
UENUM(BlueprintType)
enum class EBossModifierCategory : uint8
{
	PlayerDebuff    UMETA(DisplayName="Player Debuff"),     // stat reduction, restricted ability
	EnemySpawns     UMETA(DisplayName="Enemy Spawns"),      // adds enemies to the boss room
	Environmental   UMETA(DisplayName="Environmental"),     // arena hazards, shrinking space, etc.
	BossEnhancement UMETA(DisplayName="Boss Enhancement"),  // extra phase, resistance, new attacks
};

// Defines a single boss run modifier.
// Create one UCapsBossModifierDataAsset per modifier and add them to a pool asset.
//
// GAS effects:
//   PlayerEffect  — applied to the player immediately when they commit to entering.
//   BossEffect    — applied to the boss actor via ActivateModifiers() in BeginPlay.
//   Both are optional; EnemySpawns / Environmental modifiers typically drive behavior
//   through ModifierTag + Blueprint logic rather than GAS effects.
UCLASS(BlueprintType)
class CAPS_API UCapsBossModifierDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
	FText Description;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
	EBossModifierCategory Category = EBossModifierCategory::PlayerDebuff;

	// Icon shown on the modifier card in the reveal widget.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Display")
	TSoftObjectPtr<UTexture2D> Icon;

	// Applied to the player when they commit to entering the boss room.
	// Leave empty if this modifier doesn't directly affect the player via GAS.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> PlayerEffect;

	// Applied to the boss via ActivateModifiers().
	// Leave empty for Environmental / EnemySpawn modifiers that drive BP logic instead.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> BossEffect;

	// Tag used by room template Blueprints to trigger custom behavior.
	// e.g. "Modifier.ExtraEnemies", "Modifier.ShrinkingArena"
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	FName ModifierTag;
};
