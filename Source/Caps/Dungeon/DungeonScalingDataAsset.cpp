#include "Dungeon/DungeonScalingDataAsset.h"

const FDungeonScalingTier& UDungeonScalingDataAsset::GetTierForRoom(int32 RoomsCompleted) const
{
	static const FDungeonScalingTier DefaultTier;
	if (Tiers.IsEmpty()) return DefaultTier;

	const FDungeonScalingTier* Active = &Tiers[0];
	for (const FDungeonScalingTier& Tier : Tiers)
	{
		if (RoomsCompleted >= Tier.StartAtRoom)
			Active = &Tier;
	}
	return *Active;
}
