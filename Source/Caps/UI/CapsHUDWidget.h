#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayEffectTypes.h"
#include "CapsHUDWidget.generated.h"

class ACapsDungeonGenerator;
class UCapsReinforcementComponent;

// C++ base for the in-run HUD.
// Binds to GAS health changes (immediate) and polls room index on a short interval.
// Also binds to UCapsReinforcementComponent::OnWaveIncoming to fire a warning event.
//
// Blueprint implementation guide:
//   - Override OnHealthChanged to animate the health bar.
//   - Override OnRoomChanged to update the room number / progress display.
//   - Override OnReinforcementWarning to flash a "Reinforcements Incoming!" banner.
//   - Override OnExtractionAvailable to show the extraction prompt when near a trigger.
//     Call it from Blueprint (e.g. from the extraction trigger's overlap event).
UCLASS(Abstract, Blueprintable)
class CAPS_API UCapsHUDWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// ── Blueprint events ────────────────────────────────────────────────────────

	// Health attribute changed — animate your health bar here.
	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void OnHealthChanged(float CurrentHealth, float MaxHealth);

	// Player moved to a new room — update room number / depth display.
	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void OnRoomChanged(int32 NewRoomIndex);

	// A reinforcement wave is about to spawn — show a warning banner.
	UFUNCTION(BlueprintImplementableEvent, Category="HUD")
	void OnReinforcementWarning();

	// Call from Blueprint (via extraction trigger overlap) to show the prompt.
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category="HUD")
	void OnExtractionAvailable(bool bAvailable);

private:
	UPROPERTY()
	TObjectPtr<ACapsDungeonGenerator> CachedGenerator;

	float RefreshAccumulator = 0.f;
	static constexpr float RefreshInterval = 0.15f;

	int32 LastRoomIndex = -1;

	// Called by the GAS attribute change delegate — immediate, no polling.
	void OnHealthAttributeChanged(const FOnAttributeChangeData& Data);

	// Called by UCapsReinforcementComponent::OnWaveIncoming.
	UFUNCTION()
	void HandleWaveIncoming();

	void PollDungeonState();
	void BindHealthDelegate();
	void BindReinforcementDelegate();
};
