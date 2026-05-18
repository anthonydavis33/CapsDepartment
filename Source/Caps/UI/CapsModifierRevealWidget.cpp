#include "UI/CapsModifierRevealWidget.h"
#include "Boss/CapsBossDoorActor.h"

void UCapsModifierRevealWidget::InitializeModifiers(const TArray<UCapsBossModifierDataAsset*>& Modifiers,
	ACapsBossDoorActor* Door)
{
	OwningDoor = Door;
	OnModifiersReady(Modifiers);
}

void UCapsModifierRevealWidget::CommitToEntry()
{
	if (OwningDoor)
		OwningDoor->OnPlayerCommittedEntry();
	// OnPlayerCommittedEntry handles removing this widget from the viewport.
}

void UCapsModifierRevealWidget::Retreat()
{
	if (OwningDoor)
		OwningDoor->OnPlayerRetreated();
	// OnPlayerRetreated handles removing this widget from the viewport.
}
