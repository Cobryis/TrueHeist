// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/Tasks/BTTask_RotateToFaceBBEntry.h"
#include "THRotateToTask.generated.h"

/**
 * Simply changes RotateToFaceBBEntry from using GetActorForwardVector to GetViewRotation.
 * This was done so that the actor's body doesn't have to be rotated to be looking at the target/location,
 * just the actor's head. This assume's GetViewRotation does NOT use the control rotation! This would not
 * work if it GetViewRotation did use the control rotation because that rotates to focal points instantly.
 * THCharacter and the AI character have overridden GetViewRotation so the view comes from the pawn and 
 * not the controller.
 */
UCLASS()
class TRUEHEIST_API UTHRotateToTask : public UBTTask_RotateToFaceBBEntry
{
	GENERATED_BODY()

public:

	UTHRotateToTask(const FObjectInitializer& ObjectInitializer);

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void DescribeRuntimeValues(const UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTDescriptionVerbosity::Type Verbosity, TArray<FString>& Values) const override;

};
