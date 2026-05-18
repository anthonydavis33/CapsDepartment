#include "Dungeon/CapsRoomTileComponent.h"

UCapsRoomTileComponent::UCapsRoomTileComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

TArray<FString> UCapsRoomTileComponent::GetSlotTypeOptions() const
{
	TArray<FString> Options;
	for (const FName& Slot : UCapsGameSettings::Get()->TileSlotTypes)
		Options.Add(Slot.ToString());
	return Options;
}
