

#include "THPlayerController.h"

#include "Gameframework/SpringArmComponent.h"

#include "THCharacter.h"

ATHPlayerController::ATHPlayerController()
{

}

void ATHPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	ControlledCharacter = Cast<ATHCharacter>(InPawn);
	ActionData.Clear();
}

void ATHPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAction(TEXT("PerformAction"), EInputEvent::IE_Released, this, &ATHPlayerController::PerformActionReleased);
}

void ATHPlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (ControlledCharacter)
	{
		const float CameraBoomLength = ControlledCharacter->GetCameraBoom()->TargetArmLength; // #todo: this does not necessarily match up with the actual camera position
		FVector OutCameraLoc;
		FRotator OutCameraRot;
		PlayerCameraManager->GetCameraViewPoint(OutCameraLoc, OutCameraRot);

		FHitResult OutHit;
		if (GetWorld()->LineTraceSingleByChannel(OutHit, OutCameraLoc, OutCameraLoc + OutCameraRot.Vector() * (ActionDistance + CameraBoomLength), ECC_Actionable))
		{
			if (ActionData.CurrentActionableActor != OutHit.GetActor())
			{
				ActionData.Clear();
			}

			if (OutHit.Actor.IsValid(false) && OutHit.GetActor()->GetClass()->ImplementsInterface(UActionInterface::StaticClass()))
			{
				ActionData.CurrentActionableActor = OutHit.GetActor();
				ActionData.bHasAction = IActionInterface::Execute_HasAction(ActionData.CurrentActionableActor);
				ActionData.DisplayText = IActionInterface::Execute_GetActionText(ActionData.CurrentActionableActor);
				ActionData.ActionPrimitive = IActionInterface::Execute_GetActionPrimitive(ActionData.CurrentActionableActor);
				if (ActionData.ActionPrimitive)
				{
					ActionData.ActionPrimitive->SetCustomDepthStencilValue(8);
				}
			}
		}
		else
		{
			ActionData.Clear();
		}
	}
}

void ATHPlayerController::PerformActionReleased()
{
	if (ActionData.bHasAction)
	{
		IActionInterface::Execute_PerformAction(ActionData.CurrentActionableActor, ControlledCharacter);
	}
}
