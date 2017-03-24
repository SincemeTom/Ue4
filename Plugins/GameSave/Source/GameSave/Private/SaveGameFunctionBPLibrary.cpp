// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

#include "GameSavePrivatePCH.h"
#include "SaveGameFunctionBPLibrary.h"
#include "SaveGameArchive.h"
#include "Runtime/Engine/Public/SaveGameSystem.h"
#include "Runtime/Engine/Public/PlatformFeatures.h"

USaveGameFunctionBPLibrary::USaveGameFunctionBPLibrary(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}


bool USaveGameFunctionBPLibrary::SaveGame(UObject * SaveData, FString SlotName)
{
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (SaveSystem && SaveData && (SlotName.Len() > 0))
	{
		FSaveGameWriteArchive Archive;
		Archive << SaveData;
		return SaveSystem->SaveGame(false, *SlotName, 0, Archive);
	}
	return false;
}


bool USaveGameFunctionBPLibrary::LoadGame(UObject* & SaveData, FString SlotName)
{
	ISaveGameSystem* SaveSystem = IPlatformFeaturesModule::Get().GetSaveGameSystem();
	if (SaveSystem && (SlotName.Len() > 0))
	{
		TArray<uint8> ObjectBytes;
		bool bSuccess = SaveSystem->LoadGame(false, *SlotName, 0, ObjectBytes);
		if (bSuccess)
		{
			FSaveGameReadArchive Archive(ObjectBytes, true);
			Archive << SaveData;
			return true;
		}
	}
	return false;
}

