#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Cooking/CapsRecipeBook.h"
#include "Cooking/CookingTypes.h"
#include "CapsRunSaveGame.generated.h"

// Persistent save data for a single player profile.
//
// Stored data survives all level transitions and game restarts.
// Loaded on GameInstance::Init(), written before every hub return.
//
// Access via UCapsGameInstance::GetActiveSave() or use the helpers
// SaveCurrentRun() / RestoreInventoryFromSave() on the game instance.
UCLASS()
class CAPS_API UCapsRunSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	// Slot name used for all UGameplayStatics save/load calls.
	static const FString SaveSlotName;

	// Always slot 0 — single-player, no user-index support needed.
	static constexpr int32 SaveUserIndex = 0;

	// ── Inventory ─────────────────────────────────────────────────────────────

	// The player's hub ingredient stock. Maps IngredientInstance (ID + Quality) → count.
	// Written from UCapsInventoryComponent::BaseStock before each hub return.
	UPROPERTY(SaveGame)
	TMap<FIngredientInstance, int32> BaseStock;

	// ── Progression ───────────────────────────────────────────────────────────

	// True once the player has finished (win or die) their first dungeon run.
	// Unlocks the boss door in the hub.
	UPROPERTY(SaveGame)
	bool bFirstRunComplete = false;

	// ── Codex ─────────────────────────────────────────────────────────────────

	// Tracks every ingredient and reaction the player has discovered.
	// Used by the recipe book / codex UI.
	UPROPERTY(SaveGame)
	FCapsRecipeBook RecipeBook;
};
