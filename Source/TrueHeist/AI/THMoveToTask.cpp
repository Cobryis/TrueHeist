

#include "THMoveToTask.h"

UTHMoveToTask::UTHMoveToTask(const FObjectInitializer& ObjectIntializer)
	: Super(ObjectIntializer)
{
	NodeName = TEXT("Dynamic Move To");

	bObserveBlackboardValue = true; // why is this not on by default!

	BlackboardKey = FBlackboardKeySelector(); // reset filters so we can use TargetOrLocation
	BlackboardKey.AllowNoneAsValue(true);
}
