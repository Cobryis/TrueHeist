

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_MoveTo.h"
#include "THMoveToTask.generated.h"

/**
 * 
 */
UCLASS()
class TRUEHEIST_API UTHMoveToTask : public UBTTask_MoveTo
{
	GENERATED_BODY()

public:

	UTHMoveToTask(const FObjectInitializer& ObjectIntializer);
	
};
