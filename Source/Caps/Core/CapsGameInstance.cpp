#include "Core/CapsGameInstance.h"
#include "Kismet/GameplayStatics.h"

UCapsGameInstance* UCapsGameInstance::Get(const UObject* WorldContextObject)
{
	return Cast<UCapsGameInstance>(UGameplayStatics::GetGameInstance(WorldContextObject));
}

void UCapsGameInstance::SelectCharacter(UCapsCharacterDataAsset* CharacterData)
{
	SelectedCharacter = CharacterData;
}

void UCapsGameInstance::SetRunState(ECapsRunState NewState)
{
	RunState = NewState;
}

void UCapsGameInstance::NotifyRunEnded()
{
	bFirstRunComplete = true;
	RunState = ECapsRunState::Hub;
}
