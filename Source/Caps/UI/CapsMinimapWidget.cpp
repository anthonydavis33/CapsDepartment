#include "UI/CapsMinimapWidget.h"
#include "Dungeon/CapsDungeonGenerator.h"
#include "Kismet/GameplayStatics.h"

void UCapsMinimapWidget::NativeConstruct()
{
	Super::NativeConstruct();
	CachedGenerator = Cast<ACapsDungeonGenerator>(
		UGameplayStatics::GetActorOfClass(GetWorld(), ACapsDungeonGenerator::StaticClass()));
}

void UCapsMinimapWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	RefreshAccumulator += InDeltaTime;
	if (RefreshAccumulator < RefreshInterval) return;
	RefreshAccumulator = 0.f;

	LastData = GetMinimapData();
	OnRoomDataUpdated(LastData);
}

TArray<FMinimapRoomEntry> UCapsMinimapWidget::GetMinimapData() const
{
	if (CachedGenerator)
		return CachedGenerator->GetMinimapData();
	return {};
}

