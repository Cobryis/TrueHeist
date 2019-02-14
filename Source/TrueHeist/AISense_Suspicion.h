

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISense.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISenseEvent.h"
#include "AISense_Suspicion.generated.h"

class UAIPerceptionComponent;

USTRUCT(BlueprintType)
struct TRUEHEIST_API FAISuspiciousEvent
{
	GENERATED_USTRUCT_BODY()

	typedef class UAISense_Suspicion FSenseClass;

	float Age = 0.f;

	/** if not set Instigator's location will be used */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense", meta = (UIMin = 0, ClampMin = 0))
	float Intensity = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense", meta = (UIMin = 0, ClampMin = 0))
	float MaxRange = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
	AActor* Instigator = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
	FName Tag;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
	UAIPerceptionComponent* Observer = nullptr;

	FGenericTeamId TeamIdentifier;

	FAISuspiciousEvent() {}
	FAISuspiciousEvent(AActor* InInstigator, UAIPerceptionComponent* Observer, const FVector& InLocation, float InIntensity = 1.f, FName Tag = NAME_None);

	/** Verifies and calculates derived data */
	void Compile();
};

/**
 * 
 */
UCLASS(ClassGroup = AI, Config = Game)
class TRUEHEIST_API UAISense_Suspicion : public UAISense
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FAISuspiciousEvent> SuspiciousEvents;

public:

	UAISense_Suspicion();

	void RegisterEvent(const FAISuspiciousEvent& Event);
	void RegisterEventsBatch(const TArray<FAISuspiciousEvent>& Events);

	// part of BP interface. Translates PerceptionEvent to FAINoiseEvent and call RegisterEvent(const FAINoiseEvent& Event)
	virtual void RegisterWrappedEvent(UAISenseEvent& PerceptionEvent) override;

	/**
	 * Report a noise event.
	 *
	 * @param Location Location of the event.
	 * @param Intensity 
	 * @param Instigator Actor that triggered the event.
	 * @param Observer AIController that Perceieved the event.
	 * @param Tag Identifier for the event.
	 */
	UFUNCTION(BlueprintCallable, Category = "AI|Perception", meta = (WorldContext = "WorldContextObject"))
	static void ReportSuspiciousEvent(UObject* WorldContextObject, FVector Location, float Intensity = 1.f, AActor* Instigator = nullptr, UAIPerceptionComponent* Observer = nullptr, FName Tag = NAME_None);

protected:

	virtual float Update() override;

// 	void OnNewListenerImpl(const FPerceptionListener& NewListener);
// 	void OnListenerUpdateImpl(const FPerceptionListener& UpdatedListener);
// 	void OnListenerRemovedImpl(const FPerceptionListener& UpdatedListener);

};

UCLASS()
class TRUEHEIST_API UAISenseEvent_Suspicion : public UAISenseEvent
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sense")
	FAISuspiciousEvent Event;

public:
	UAISenseEvent_Suspicion(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()) {}
	virtual FAISenseID GetSenseID() const override { return UAISense::GetSenseID<UAISense_Suspicion>(); }

	FORCEINLINE FAISuspiciousEvent GetSuspiciousEvent()
	{
		Event.Compile();
		return Event;
	}
};

/**
 *
 */
UCLASS()
class TRUEHEIST_API UAISenseConfig_Suspicion : public UAISenseConfig
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Sense", NoClear, config)
	TSubclassOf<UAISense_Suspicion> Implementation;

public:

	virtual TSubclassOf<UAISense> GetSenseImplementation() const override { return *Implementation; }

#if WITH_GAMEPLAY_DEBUGGER
	virtual void DescribeSelfToGameplayDebugger(const UAIPerceptionComponent* PerceptionComponent, class FGameplayDebuggerCategory* DebuggerCategory) const override;
#endif // WITH_GAMEPLAY_DEBUGGER
};

