// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.
#pragma once
#include "GameFramework/PlayerController.h"
#include "TestThirdProjectPlayerController.generated.h"

UCLASS()
class ATestThirdProjectPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ATestThirdProjectPlayerController();

	void SetPathFollowingComp(UPathFollowingComponent* InPathFollowingComp);
	virtual void InitNavigationControl(UPathFollowingComponent*& PathFollowingComp) override;
protected:
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	// End PlayerController interface


	/** Resets HMD orientation in VR. */
	void OnResetVR();
protected:
	UPROPERTY()
	UPathFollowingComponent* PathFollowingComponent;
};


