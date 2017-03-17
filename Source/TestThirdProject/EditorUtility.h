// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GlobalEditorUtilityBase.h"
#include "EditorUtility.generated.h"

/**
 * 
 */
UCLASS()
class TESTTHIRDPROJECT_API UEditorUtility : public UGlobalEditorUtilityBase
{
	GENERATED_BODY()
	
public:
	
	UFUNCTION()
		void RenameSelectedAssets();


private:
	TArray<class FAssetData> GetSelectionAssets();
	
public:
	//UFUNCTION()
	
};
