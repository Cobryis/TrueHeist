

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "ActionInterface.h"

#include "THPlayerController.generated.h"

UCLASS()
class TRUEHEIST_API ATHPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	ATHPlayerController();

	virtual void SetPawn(APawn* InPawn) override;
	virtual void SetupInputComponent() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void PerformActionReleased();

protected:

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly, Category = "Action")
	FActionData ActionData;

	UPROPERTY(EditAnywhere, Category = "Action")
	float ActionDistance = 500.f;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "TrueHeist")
	class ATHCharacter* ControlledCharacter;
	
};
