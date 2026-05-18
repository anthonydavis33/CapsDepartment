#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Dungeon/CapsRoomTypes.h"
#include "CapsMinimapWidget.generated.h"

class ACapsDungeonGenerator;

// C++ base for the minimap widget. Provides GetMinimapData() which Blueprint
// uses to draw room icons each frame.
//
// Blueprint implementation guide:
//   - Override OnRoomDataUpdated to rebuild the visual layout
//   - Use RoomType + bIsCurrent + bRevealedByProximity to pick icon style
//   - Call RefreshMinimap() on a 0.1s timer (not every frame — data rarely changes)
//   - Safe rooms revealed by proximity can show a distinct "?" or compass icon
UCLASS(Abstract, Blueprintable)
class CAPS_API UCapsMinimapWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// Returns the current minimap snapshot. Call this to rebuild the display.
	UFUNCTION(BlueprintCallable, Category="Minimap")
	TArray<FMinimapRoomEntry> GetMinimapData() const;

	// Pixel size of each room cell on the minimap canvas.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap")
	float RoomCellSize = 20.f;

	// Pixel gap between room cells.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Minimap")
	float RoomCellGap = 4.f;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// Called whenever the minimap data has changed. Override in Blueprint
	// to rebuild the visual layout from the new entry list.
	UFUNCTION(BlueprintImplementableEvent, Category="Minimap")
	void OnRoomDataUpdated(const TArray<FMinimapRoomEntry>& Entries);

private:
	UPROPERTY()
	TObjectPtr<ACapsDungeonGenerator> CachedGenerator;

	// Most recent snapshot passed to OnRoomDataUpdated. Exposed so Blueprint
	// can query the last known state without triggering a new refresh.
	UPROPERTY(BlueprintReadOnly, Category="Minimap", meta=(AllowPrivateAccess="true"))
	TArray<FMinimapRoomEntry> LastData;

	float RefreshAccumulator = 0.f;

	static constexpr float RefreshInterval = 0.15f;
};
