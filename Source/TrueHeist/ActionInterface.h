

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ActionInterface.generated.h"

#define ECC_Actionable ECC_GameTraceChannel1

USTRUCT(BlueprintType)
struct FActionData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Action")
	AActor* CurrentActionableActor = nullptr;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Action")
	bool bHasAction = false;

	UPROPERTY(BlueprintReadOnly, Transient, Category = "Action")
	FText DisplayText;

	bool IsValid()
	{
		return CurrentActionableActor != nullptr;
	}

	void Clear()
	{
		if (IsValid())
		{
			CurrentActionableActor = nullptr;
			bHasAction = false;
			DisplayText = FText::GetEmpty();
		}
	}

};

UINTERFACE(MinimalAPI)
class UActionInterface : public UInterface
{
	GENERATED_BODY()
};

class TRUEHEIST_API IActionInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	bool HasAction() const;
	virtual bool HasAction_Implementation() const { return false; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	FText GetActionText() const;
	virtual FText GetActionText_Implementation() const { return FText::FromString(TEXT("Perform Action")); }

	// Get the primitive that should be highlighted
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	UPrimitiveComponent* GetActionPrimitive() const;
	virtual UPrimitiveComponent* GetActionPrimitive_Implementation() const { return nullptr; }

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Action")
	void PerformAction(APawn* Instigator);
	virtual void PerformAction_Implementation(APawn* Instigator) PURE_VIRTUAL(IActionInterface::PerformAction_Implementation, return;);

};
