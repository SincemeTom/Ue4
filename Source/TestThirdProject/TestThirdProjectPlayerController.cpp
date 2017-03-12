// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#include "TestThirdProject.h"
#include "TestThirdProjectPlayerController.h"
#include "AI/Navigation/NavigationSystem.h"
#include "Runtime/Engine/Classes/Components/DecalComponent.h"
#include "Kismet/HeadMountedDisplayFunctionLibrary.h"
#include "Navigation/CrowdFollowingComponent.h"
#include "TestThirdProjectCharacter.h"


ATestThirdProjectPlayerController::ATestThirdProjectPlayerController()
{
	DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void ATestThirdProjectPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	// keep updating the destination every tick while desired
}

void ATestThirdProjectPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAction("ResetVR", IE_Pressed, this, &ATestThirdProjectPlayerController::OnResetVR);
}


void ATestThirdProjectPlayerController::OnResetVR()
{
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}
void ATestThirdProjectPlayerController::InitNavigationControl(UPathFollowingComponent*& PathFollowingComp)
{
	PathFollowingComp = PathFollowingComponent;
}
void ATestThirdProjectPlayerController::SetPathFollowingComp(UPathFollowingComponent* InPathFollowingComp) 
{ 
	PathFollowingComponent = InPathFollowingComp; 
}