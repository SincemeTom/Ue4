// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/SaveGame.h"
#include "MySaveGame.generated.h"

/**
 * 
 */
UCLASS()
class TESTTHIRDPROJECT_API UMySaveGame : public USaveGame
{
	GENERATED_BODY()
public:
	UPROPERTY()
	AActor* SaveActor;
	UPROPERTY()
	FString Name;
	
};
