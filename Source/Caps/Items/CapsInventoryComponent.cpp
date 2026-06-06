#include "Items/CapsInventoryComponent.h"
#include "Cooking/IngredientDataAsset.h"
#include "Cooking/ReactionDataAsset.h"
#include "Core/CapsGameInstance.h"
#include "Core/CapsRunSaveGame.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "Engine/AssetManager.h"
#include "Caps.h"

const ECookingSlot UCapsInventoryComponent::AllSlots[] = {
	ECookingSlot::MainDish,
	ECookingSlot::Appetizer,
	ECookingSlot::Dessert,
	ECookingSlot::Side,
	ECookingSlot::Drink
};

UCapsInventoryComponent::UCapsInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

// ── Base Stock ────────────────────────────────────────────────────────────────

void UCapsInventoryComponent::AddToBaseStock(FName IngredientID, EIngredientQuality Quality, int32 Quantity)
{
	if (IngredientID.IsNone() || Quantity <= 0) return;
	BaseStock.FindOrAdd(FIngredientInstance{IngredientID, Quality}) += Quantity;
}

bool UCapsInventoryComponent::RemoveFromBaseStock(FName IngredientID, EIngredientQuality Quality, int32 Quantity)
{
	const FIngredientInstance Key{IngredientID, Quality};
	int32* Current = BaseStock.Find(Key);
	if (!Current || *Current < Quantity) return false;
	*Current -= Quantity;
	if (*Current <= 0) BaseStock.Remove(Key);
	return true;
}

int32 UCapsInventoryComponent::GetBaseStockQuantity(FName IngredientID, EIngredientQuality Quality) const
{
	const int32* Qty = BaseStock.Find(FIngredientInstance{IngredientID, Quality});
	return Qty ? *Qty : 0;
}

int32 UCapsInventoryComponent::GetTotalBaseStockQuantity(FName IngredientID) const
{
	int32 Total = 0;
	for (const auto& Pair : BaseStock)
	{
		if (Pair.Key.IngredientID == IngredientID)
			Total += Pair.Value;
	}
	return Total;
}

// ── Cooking Station ───────────────────────────────────────────────────────────

bool UCapsInventoryComponent::SlotIngredient(FName IngredientID, EIngredientQuality Quality, ECookingSlot Dish, ESubSlotType SubSlot, int32 SubSlotIndex)
{
	if (!RemoveFromBaseStock(IngredientID, Quality, 1)) return false;

	for (const FSlottedIngredient& Existing : SlottedIngredients)
	{
		if (Existing.DishSlot == Dish && Existing.SubSlot == SubSlot && Existing.SubSlotIndex == SubSlotIndex)
		{
			AddToBaseStock(IngredientID, Quality, 1);
			return false;
		}
	}

	FSlottedIngredient& New = SlottedIngredients.AddDefaulted_GetRef();
	New.IngredientID  = IngredientID;
	New.Quality       = Quality;
	New.DishSlot      = Dish;
	New.SubSlot       = SubSlot;
	New.SubSlotIndex  = SubSlotIndex;
	return true;
}

void UCapsInventoryComponent::UnslotIngredient(ECookingSlot Dish, ESubSlotType SubSlot, int32 SubSlotIndex)
{
	for (int32 i = SlottedIngredients.Num() - 1; i >= 0; i--)
	{
		const FSlottedIngredient& S = SlottedIngredients[i];
		if (S.DishSlot == Dish && S.SubSlot == SubSlot && S.SubSlotIndex == SubSlotIndex)
		{
			AddToBaseStock(S.IngredientID, S.Quality, 1);
			SlottedIngredients.RemoveAt(i);
			return;
		}
	}
}

void UCapsInventoryComponent::ClearDish(ECookingSlot Dish)
{
	for (int32 i = SlottedIngredients.Num() - 1; i >= 0; i--)
	{
		if (SlottedIngredients[i].DishSlot == Dish)
		{
			AddToBaseStock(SlottedIngredients[i].IngredientID, SlottedIngredients[i].Quality, 1);
			SlottedIngredients.RemoveAt(i);
		}
	}
}

TArray<FSlottedIngredient> UCapsInventoryComponent::GetDishIngredients(ECookingSlot Dish) const
{
	TArray<FSlottedIngredient> Result;
	for (const FSlottedIngredient& S : SlottedIngredients)
	{
		if (S.DishSlot == Dish) Result.Add(S);
	}
	return Result;
}

// ── Meal & Run ────────────────────────────────────────────────────────────────

void UCapsInventoryComponent::EatMeal()
{
	ActiveMeal = SlottedIngredients;
	SlottedIngredients.Empty();
	RunPickups.Empty();

	UAbilitySystemComponent* ASC = nullptr;
	if (IAbilitySystemInterface* Owner = Cast<IAbilitySystemInterface>(GetOwner()))
	{
		ASC = Owner->GetAbilitySystemComponent();
	}

	if (!ASC)
	{
		UE_LOG(LogCaps, Warning, TEXT("UCapsInventoryComponent::EatMeal — owner has no ASC."));
		return;
	}

	for (ECookingSlot Dish : AllSlots)
	{
		ApplyDishEffects(Dish, ASC);
	}

	// Record every consumed ingredient in the codex so the recipe book UI
	// can show which slots each ingredient has been used in.
	if (UCapsGameInstance* GI = UCapsGameInstance::Get(GetWorld()))
	{
		if (UCapsRunSaveGame* Save = GI->GetActiveSave())
		{
			for (const FSlottedIngredient& Slot : ActiveMeal)
			{
				if (!Slot.IngredientID.IsNone())
					Save->RecipeBook.MarkIngredientUsed(Slot.IngredientID, Slot.DishSlot);
			}
		}
	}
}

void UCapsInventoryComponent::AddRunPickup(FName IngredientID, EIngredientQuality Quality, int32 Quantity)
{
	if (IngredientID.IsNone() || Quantity <= 0) return;
	RunPickups.FindOrAdd(FIngredientInstance{IngredientID, Quality}) += Quantity;
}

// ── Extraction / Death ────────────────────────────────────────────────────────

void UCapsInventoryComponent::HandleExtraction()
{
	for (auto& Pair : RunPickups)
	{
		AddToBaseStock(Pair.Key.IngredientID, Pair.Key.Quality, Pair.Value);
	}
	RunPickups.Empty();
	ActiveMeal.Empty();
}

void UCapsInventoryComponent::HandleDeath()
{
	// Merge run pickups into base stock for the loss calculation.
	TMap<FIngredientInstance, int32> Combined = BaseStock;
	for (auto& Pair : RunPickups)
	{
		Combined.FindOrAdd(Pair.Key) += Pair.Value;
	}
	RunPickups.Empty();
	ActiveMeal.Empty();

	// Flatten to individual items so each unit has an equal chance of being lost.
	// Quality is preserved — a Prime Pepper and a Table Pepper are separate entries.
	TArray<FIngredientInstance> ItemPool;
	for (auto& Pair : Combined)
	{
		for (int32 i = 0; i < Pair.Value; i++)
		{
			ItemPool.Add(Pair.Key);
		}
	}

	// Fisher-Yates shuffle
	for (int32 i = ItemPool.Num() - 1; i > 0; i--)
	{
		int32 j = FMath::RandRange(0, i);
		ItemPool.Swap(i, j);
	}

	// Keep the top 50%, rounded up
	int32 KeepCount = FMath::CeilToInt(ItemPool.Num() * 0.5f);
	BaseStock.Empty();
	for (int32 i = 0; i < KeepCount; i++)
	{
		BaseStock.FindOrAdd(ItemPool[i])++;
	}
}

// ── Private ───────────────────────────────────────────────────────────────────

UIngredientDataAsset* UCapsInventoryComponent::LoadIngredient(FName IngredientID) const
{
	FPrimaryAssetId AssetId("Ingredient", IngredientID);
	FSoftObjectPath AssetPath = UAssetManager::Get().GetPrimaryAssetPath(AssetId);
	return Cast<UIngredientDataAsset>(AssetPath.TryLoad());
}

void UCapsInventoryComponent::ApplyDishEffects(ECookingSlot Dish, UAbilitySystemComponent* ASC)
{
	FGameplayTagContainer DishTags;

	for (const FSlottedIngredient& Slot : ActiveMeal)
	{
		if (Slot.DishSlot != Dish) continue;

		UIngredientDataAsset* Ingredient = LoadIngredient(Slot.IngredientID);
		if (!Ingredient) continue;

		DishTags.AppendTags(Ingredient->FoodTags);

		const FIngredientSlotEffect* Effect = Ingredient->SlotEffects.Find(Dish);
		if (Effect && Effect->GameplayEffect)
		{
			// Quality maps to GE level: Table=1, Choice=2, Prime=3.
			// Parametric GEs (stat bumps) read this level via their CurveTable row.
			// Non-parametric GEs (tag grants like GE_Grant_Fire) ignore level entirely.
			// Clamp to 1 minimum — Level 0 is invalid in GAS.
			const float GELevel = FMath::Max(1.f, static_cast<float>(Slot.Quality));
			FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(Effect->GameplayEffect, GELevel, Context);
			if (Spec.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}
	}

	ResolveReactions(Dish, DishTags, ASC);
}

void UCapsInventoryComponent::ResolveReactions(ECookingSlot Dish, const FGameplayTagContainer& DishTags, UAbilitySystemComponent* ASC)
{
	for (UReactionDataAsset* Reaction : KnownReactions)
	{
		if (!Reaction) continue;

		if (Reaction->ApplicableDishTypes.Num() > 0 && !Reaction->ApplicableDishTypes.Contains(Dish))
		{
			continue;
		}

		if (DishTags.HasAny(Reaction->BlockingFoodTags)) continue;
		if (!DishTags.HasAll(Reaction->RequiredFoodTags)) continue;

		if (Reaction->ReactionEffect)
		{
			FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
			FGameplayEffectSpecHandle Spec = ASC->MakeOutgoingSpec(Reaction->ReactionEffect, 1.f, Context);
			if (Spec.IsValid())
			{
				ASC->ApplyGameplayEffectSpecToSelf(*Spec.Data.Get());
			}
		}

		UE_LOG(LogCaps, Log, TEXT("Reaction fired in %s: %s"),
			*UEnum::GetValueAsString(Dish),
			*Reaction->ReactionName.ToString());

		// Record the discovery in the player's recipe book codex.
		if (UCapsGameInstance* GI = UCapsGameInstance::Get(GetWorld()))
		{
			if (UCapsRunSaveGame* Save = GI->GetActiveSave())
				Save->RecipeBook.MarkReactionDiscovered(Reaction->GetFName(), 0);
		}
	}
}
