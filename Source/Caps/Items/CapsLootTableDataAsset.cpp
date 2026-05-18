#include "Items/CapsLootTableDataAsset.h"

TArray<FName> UCapsLootTableDataAsset::RollDrops() const
{
	TArray<FName> Result;
	if (Entries.IsEmpty()) return Result;

	float TotalWeight = 0.f;
	for (const FLootEntry& Entry : Entries)
		TotalWeight += FMath::Max(0.f, Entry.Weight);

	if (TotalWeight <= 0.f) return Result;

	const int32 NumDrops = FMath::RandRange(MinDrops, MaxDrops);
	for (int32 i = 0; i < NumDrops; ++i)
	{
		float Roll = FMath::FRandRange(0.f, TotalWeight);
		float Accumulated = 0.f;

		for (const FLootEntry& Entry : Entries)
		{
			Accumulated += Entry.Weight;
			if (Roll <= Accumulated)
			{
				const int32 Count = FMath::RandRange(Entry.MinCount, Entry.MaxCount);
				for (int32 j = 0; j < Count; ++j)
					Result.Add(Entry.IngredientID);
				break;
			}
		}
	}

	return Result;
}
