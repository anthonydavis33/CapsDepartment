#include "Cooking/CapsRecipeBook.h"

bool FCapsRecipeBook::HasDiscoveredIngredient(FName IngredientID) const
{
	return DiscoveredIngredients.ContainsByPredicate([&](const FIngredientCodexEntry& E)
	{
		return E.IngredientID == IngredientID;
	});
}

bool FCapsRecipeBook::HasDiscoveredReaction(FName ReactionID) const
{
	return DiscoveredReactions.ContainsByPredicate([&](const FDiscoveredReaction& R)
	{
		return R.ReactionID == ReactionID;
	});
}

void FCapsRecipeBook::MarkIngredientUsed(FName IngredientID, ECookingSlot InSlot)
{
	FIngredientCodexEntry* Existing = DiscoveredIngredients.FindByPredicate([&](const FIngredientCodexEntry& E)
	{
		return E.IngredientID == IngredientID;
	});

	if (Existing)
	{
		Existing->UsedInSlots.AddUnique(InSlot);
	}
	else
	{
		FIngredientCodexEntry NewEntry;
		NewEntry.IngredientID = IngredientID;
		NewEntry.UsedInSlots.Add(InSlot);
		DiscoveredIngredients.Add(NewEntry);
	}
}

void FCapsRecipeBook::MarkReactionDiscovered(FName ReactionID, int32 RunIndex)
{
	if (!HasDiscoveredReaction(ReactionID))
	{
		FDiscoveredReaction NewReaction;
		NewReaction.ReactionID = ReactionID;
		NewReaction.DiscoveredOnRun = RunIndex;
		DiscoveredReactions.Add(NewReaction);
	}
}
