// Fill out your copyright notice in the Description page of Project Settings.

#include "GameSavePrivatePCH.h"
#include "SaveGameArchive.h"

FArchive& FSaveGameWriteArchive::operator<<(UObject*& Obj)
{
	FString ClassPath;

	if (Obj != nullptr || Obj->IsValidLowLevel())
	{
		UClass* ObjectClass = Obj->GetClass();
		FStringAssetReference ThePath = FStringAssetReference(ObjectClass);
		if (ThePath.IsValid())
		{
			ClassPath = ObjectClass->GetClass()->GetDescription();
			ClassPath += "'";
			ClassPath += ThePath.ToString();
			ClassPath += "'";
			(*this) << ClassPath;
			Obj->Serialize((*this));
			return *this;
		}
	}
	(*this) << ClassPath;
	return *this;
}


FArchive& FSaveGameReadArchive::operator<<(UObject*& Obj)
{
	// load
	FString ClassPath;
	(*this) << ClassPath;
	if (ClassPath.Len() > 0)
	{
		UClass * CustomClass = Cast<UClass>(StaticLoadObject(UClass::StaticClass(), nullptr, *ClassPath, NULL, LOAD_None));
		if (CustomClass != nullptr || CustomClass->IsValidLowLevel())
		{
			Obj = NewObject<UObject>(GetTransientPackage(), CustomClass);
			Obj->AddToRoot();
			Obj->Serialize((*this));
		}
	}
	return *this;
}