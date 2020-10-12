// Copyright Epic Games, Inc. All Rights Reserved.

#include "PA3GameMode.h"
#include "PA3PlayerController.h"
#include "PA3Character.h"
#include "UObject/ConstructorHelpers.h"

APA3GameMode::APA3GameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = APA3PlayerController::StaticClass();

	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}