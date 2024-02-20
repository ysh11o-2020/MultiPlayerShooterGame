// Copyright Epic Games, Inc. All Rights Reserved.

#include "SteamSessionDemoGameMode.h"
#include "SteamSessionDemoCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASteamSessionDemoGameMode::ASteamSessionDemoGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
