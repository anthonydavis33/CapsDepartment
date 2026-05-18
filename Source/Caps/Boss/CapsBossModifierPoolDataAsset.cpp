#include "Boss/CapsBossModifierPoolDataAsset.h"
#include "Boss/CapsBossModifierDataAsset.h"
#include "Algo/RandomShuffle.h"

TArray<UCapsBossModifierDataAsset*> UCapsBossModifierPoolDataAsset::DrawModifiers(int32 Count) const
{
	// Build an index list, shuffle it, then pick the first Count valid entries.
	TArray<int32> Indices;
	Indices.SetNum(Modifiers.Num());
	for (int32 i = 0; i < Modifiers.Num(); ++i) Indices[i] = i;
	Algo::RandomShuffle(Indices);

	TArray<UCapsBossModifierDataAsset*> Result;
	const int32 DrawCount = FMath::Min(Count, Indices.Num());
	for (int32 i = 0; i < DrawCount; ++i)
	{
		if (UCapsBossModifierDataAsset* Mod = Modifiers[Indices[i]].Get())
			Result.Add(Mod);
	}
	return Result;
}
