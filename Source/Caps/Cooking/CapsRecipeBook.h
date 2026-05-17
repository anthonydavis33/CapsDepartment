#pragma once

#include "CoreMinimal.h"
#include "Cooking/CookingTypes.h"
#include "CapsRecipeBook.generated.h"

// ── Ingredient Codex Entry ────────────────────────────────────────────────────
// Unlocked the first time an ingredient is placed into any slot.

USTRUCT(BlueprintType)
struct FIngredientCodexEntry
{
	GENERATED_BODY()

	// Primary asset ID of the ingredient (matches UIngredientDataAsset::GetPrimaryAssetId).
	UPROPERTY(SaveGame, BlueprintReadOnly)
	FName IngredientID;

	// Which slots the player has actually used this ingredient in.
	// Hints for that slot's EffectHint are revealed per slot, not all at once.
	UPROPERTY(SaveGame, BlueprintReadOnly)
	TArray<ECookingSlot> UsedInSlots;
};

// ── Reaction Discovery ────────────────────────────────────────────────────────

USTRUCT(BlueprintType)
struct FDiscoveredReaction
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, BlueprintReadOnly)
	FName ReactionID;

	// Which run it was first discovered in (run index, 0-based).
	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 DiscoveredOnRun = 0;
};

// ── Run History Entry ─────────────────────────────────────────────────────────
// Snapshot of a single run's meal configuration and outcome.

UENUM(BlueprintType)
enum class ERunOutcome : uint8
{
	Extracted	UMETA(DisplayName="Extracted"),
	Died		UMETA(DisplayName="Died"),
	BossKilled	UMETA(DisplayName="Boss Killed"),
};

USTRUCT(BlueprintType)
struct FDishSnapshot
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, BlueprintReadOnly)
	ECookingSlot SlotType;

	// Ordered list of ingredient IDs placed in this dish (sub-slot order preserved).
	UPROPERTY(SaveGame, BlueprintReadOnly)
	TArray<FName> IngredientIDs;

	// Reactions that fired for this dish.
	UPROPERTY(SaveGame, BlueprintReadOnly)
	TArray<FName> ReactionIDs;
};

USTRUCT(BlueprintType)
struct FRunHistoryEntry
{
	GENERATED_BODY()

	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 RunIndex = 0;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	ERunOutcome Outcome;

	UPROPERTY(SaveGame, BlueprintReadOnly)
	TArray<FDishSnapshot> Dishes;

	// Rooms cleared, enemies killed — can expand later.
	UPROPERTY(SaveGame, BlueprintReadOnly)
	int32 RoomsCleared = 0;
};

// ── Recipe Book ───────────────────────────────────────────────────────────────
// Stored on the SaveGame object. Passed to the recipe book UI widget.

USTRUCT(BlueprintType)
struct FCapsRecipeBook
{
	GENERATED_BODY()

	// Ingredients the player has encountered (placed at least once).
	UPROPERTY(SaveGame, BlueprintReadOnly)
	TArray<FIngredientCodexEntry> DiscoveredIngredients;

	// Reactions fired at least once across any run.
	UPROPERTY(SaveGame, BlueprintReadOnly)
	TArray<FDiscoveredReaction> DiscoveredReactions;

	// Full run history, newest last.
	UPROPERTY(SaveGame, BlueprintReadOnly)
	TArray<FRunHistoryEntry> RunHistory;

	int32 GetCurrentRunIndex() const { return RunHistory.Num(); }

	bool HasDiscoveredIngredient(FName IngredientID) const;
	bool HasDiscoveredReaction(FName ReactionID) const;
	void MarkIngredientUsed(FName IngredientID, ECookingSlot InSlot);
	void MarkReactionDiscovered(FName ReactionID, int32 RunIndex);
};
