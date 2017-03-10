// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "TestThirdProject.h"
#include "TestThirdProjectGameMode.h"
#include "TestThirdProjectPlayerController.h"
#include "TestThirdProjectCharacter.h"

ATestThirdProjectGameMode::ATestThirdProjectGameMode()
{
	// use our custom PlayerController class
	PlayerControllerClass = ATestThirdProjectPlayerController::StaticClass();
	DefaultPawnClass = ATestThirdProjectCharacter::StaticClass();
	// set default pawn class to our Blueprinted character
/*	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/TopDownCPP/Blueprints/TopDownCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
	*/
	
}