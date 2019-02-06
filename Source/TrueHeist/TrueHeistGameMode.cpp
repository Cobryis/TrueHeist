// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "TrueHeistGameMode.h"
#include "TrueHeistCharacter.h"
#include "UObject/ConstructorHelpers.h"

ATrueHeistGameMode::ATrueHeistGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
