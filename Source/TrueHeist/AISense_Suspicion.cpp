

#include "AISense_Suspicion.h"

#include "Perception/AIPerceptionComponent.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Sight.h"

FAISuspiciousEvent::FAISuspiciousEvent(AActor* InInstigator, UAIPerceptionComponent* InObserver, const FVector& InLocation, float InIntensity, FName InTag)
	: Age(0.f), Location(InLocation), Intensity(InIntensity)
	, Instigator(InInstigator), Tag(InTag), Observer(InObserver)
	, TeamIdentifier(FGenericTeamId::NoTeam)
{
	Compile();
}

void FAISuspiciousEvent::Compile()
{
	TeamIdentifier = FGenericTeamId::GetTeamIdentifier(Instigator);
	if (FAISystem::IsValidLocation(Location) == false && Instigator != nullptr)
	{
		Location = Instigator->GetActorLocation();
	}
}

UAISense_Suspicion::UAISense_Suspicion()
{
// 	if (HasAnyFlags(RF_ClassDefaultObject) == false)
// 	{
// 		OnNewListenerDelegate.BindUObject(this, &UAISense_Suspicion::OnNewListenerImpl);
// 		OnListenerUpdateDelegate.BindUObject(this, &UAISense_Suspicion::OnListenerUpdateImpl);
// 		OnListenerRemovedDelegate.BindUObject(this, &UAISense_Suspicion::OnListenerRemovedImpl);
// 	}
}

void UAISense_Suspicion::RegisterEvent(const FAISuspiciousEvent& Event)
{
	SuspiciousEvents.Add(Event);

	RequestImmediateUpdate();
}

void UAISense_Suspicion::RegisterEventsBatch(const TArray<FAISuspiciousEvent>& Events)
{
	SuspiciousEvents.Append(Events);

	RequestImmediateUpdate();
}

void UAISense_Suspicion::RegisterWrappedEvent(UAISenseEvent& PerceptionEvent)
{
	UAISenseEvent_Suspicion* Event = Cast<UAISenseEvent_Suspicion>(&PerceptionEvent);
	ensure(Event);
	if (Event)
	{
		RegisterEvent(Event->GetSuspiciousEvent());
	}
}

void UAISense_Suspicion::ReportSuspiciousEvent(UObject* WorldContextObject, FVector Location, float Intensity /*= 1.f*/, AActor* Instigator /*= nullptr*/, UAIPerceptionComponent* Observer /*= nullptr*/, FName Tag /*= NAME_None*/)
{
	UAIPerceptionSystem* PerceptionSystem = UAIPerceptionSystem::GetCurrent(WorldContextObject);
	if (PerceptionSystem)
	{
		FAISuspiciousEvent Event(Instigator, Observer, Location, Intensity, Tag);
		PerceptionSystem->OnEvent(Event);
	}
}

// void UAISense_Suspicion::OnNewListenerImpl(const FPerceptionListener& NewListener)
// {
// 	check(NewListener.Listener.IsValid());
// 	const UAISenseConfig_Suspicion* SenseConfig = Cast<const UAISenseConfig_Suspicion>(NewListener.Listener->GetSenseConfig(GetSenseID()));
// 	check(SenseConfig);
// 	const FDigestedHearingProperties PropertyDigest(*SenseConfig);
// 	DigestedProperties.Add(NewListener.GetListenerID(), PropertyDigest);
// }
// 
// void UAISense_Suspicion::OnListenerUpdateImpl(const FPerceptionListener& UpdatedListener)
// {
// 	// @todo add updating code here
// 	const FPerceptionListenerID ListenerID = UpdatedListener.GetListenerID();
// 
// 	if (UpdatedListener.HasSense(GetSenseID()))
// 	{
// 		const UAISenseConfig_Suspicion* SenseConfig = Cast<const UAISenseConfig_Suspicion>(UpdatedListener.Listener->GetSenseConfig(GetSenseID()));
// 		check(SenseConfig);
// 		FDigestedHearingProperties& PropertiesDigest = DigestedProperties.FindOrAdd(ListenerID);
// 		PropertiesDigest = FDigestedHearingProperties(*SenseConfig);
// 	}
// 	else
// 	{
// 		DigestedProperties.Remove(ListenerID);
// 	}
// }
// 
// void UAISense_Suspicion::OnListenerRemovedImpl(const FPerceptionListener& UpdatedListener)
// {
// 	DigestedProperties.FindAndRemoveChecked(UpdatedListener.GetListenerID());
// }

float UAISense_Suspicion::Update()
{
	AIPerception::FListenerMap& ListenersMap = *GetListeners();
	UAIPerceptionSystem* PerceptionSys = GetPerceptionSystem();


	for (AIPerception::FListenerMap::TIterator ListenerIt(ListenersMap); ListenerIt; ++ListenerIt)
	{
		FPerceptionListener& Listener = ListenerIt->Value;

		if (Listener.HasSense(GetSenseID()) == false)
		{
			// skip listeners not interested in this sense
			continue;
		}

		TArray<AActor*> CurrentlyPerceivedActors;
		Listener.Listener->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), CurrentlyPerceivedActors);
		for (const FAISuspiciousEvent& Event : SuspiciousEvents)
		{
			// if the event was caused by an observer, only stimulate that observer
			if (Event.Observer && (Event.Observer != Listener.Listener))
			{
				continue;
			}

			if (CurrentlyPerceivedActors.Contains(Event.Instigator))
			{
				FAIStimulus SuspicionOnInstigator(*this, FMath::Max(0.f, Event.Intensity), Event.Location, Listener.CachedLocation, FAIStimulus::SensingSucceeded, Event.Tag);

				// if we're already suspicious of this actor, add the old suspicion to the new
				FActorPerceptionBlueprintInfo ActorPerceptioninfo;
				Listener.Listener->GetActorsPerception(Event.Instigator, ActorPerceptioninfo);
				if (ActorPerceptioninfo.LastSensedStimuli.IsValidIndex(GetSenseID()))
				{
					FAIStimulus& OldStimulus = ActorPerceptioninfo.LastSensedStimuli[GetSenseID()];
					SuspicionOnInstigator.Strength += OldStimulus.Strength;
				}

				Listener.RegisterStimulus(Event.Instigator, SuspicionOnInstigator);
			}
		}
	}

	SuspiciousEvents.Reset();

	// return decides when next tick is going to happen
	return SuspendNextUpdate;
}

#if WITH_GAMEPLAY_DEBUGGER
void UAISenseConfig_Suspicion::DescribeSelfToGameplayDebugger(const UAIPerceptionComponent* PerceptionComponent, class FGameplayDebuggerCategory* DebuggerCategory) const
{
	if (PerceptionComponent == nullptr || DebuggerCategory == nullptr)
	{
		return;
	}
// 
// 	FColor SightRangeColor = FColor::Green;
// 	FColor LoseSightRangeColor = FColorList::NeonPink;

//	// don't call Super implementation on purpose, replace color description line
// 	DebuggerCategory->AddTextLine(
// 		FString::Printf(TEXT("%s: {%s}%s {white}rangeIN:{%s}%s {white} rangeOUT:{%s}%s"), *GetSenseName(),
// 			*GetDebugColor().ToString(), *DescribeColorHelper(GetDebugColor()),
// 			*SightRangeColor.ToString(), *DescribeColorHelper(SightRangeColor),
// 			*LoseSightRangeColor.ToString(), *DescribeColorHelper(LoseSightRangeColor))
// 	);
//
// 	const AActor* BodyActor = PerceptionComponent->GetBodyActor();
// 	if (BodyActor != nullptr)
// 	{
// 		FVector BodyLocation, BodyFacing;
// 		PerceptionComponent->GetLocationAndDirection(BodyLocation, BodyFacing);
//
// 		DebuggerCategory->AddShape(FGameplayDebuggerShape::MakeCylinder(BodyLocation, LoseSightRadius, 25.0f, LoseSightRangeColor));
// 		DebuggerCategory->AddShape(FGameplayDebuggerShape::MakeCylinder(BodyLocation, SightRadius, 25.0f, SightRangeColor));
// 
// 		const float SightPieLength = FMath::Max(LoseSightRadius, SightRadius);
// 		DebuggerCategory->AddShape(FGameplayDebuggerShape::MakeSegment(BodyLocation, BodyLocation + (BodyFacing * SightPieLength), SightRangeColor));
// 		DebuggerCategory->AddShape(FGameplayDebuggerShape::MakeSegment(BodyLocation, BodyLocation + (BodyFacing.RotateAngleAxis(PeripheralVisionAngleDegrees, FVector::UpVector) * SightPieLength), SightRangeColor));
// 		DebuggerCategory->AddShape(FGameplayDebuggerShape::MakeSegment(BodyLocation, BodyLocation + (BodyFacing.RotateAngleAxis(-PeripheralVisionAngleDegrees, FVector::UpVector) * SightPieLength), SightRangeColor));
// 
// 		DebuggerCategory->AddShape
// 	}
}
#endif // WITH_GAMEPLAY_DEBUGGER
