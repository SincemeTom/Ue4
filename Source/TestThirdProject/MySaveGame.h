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
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Save)
	AActor* SaveActor;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Save)
	FString Name;
	
};
