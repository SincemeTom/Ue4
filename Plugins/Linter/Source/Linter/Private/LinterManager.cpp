// Copyright 2016 Gamemakin LLC. All Rights Reserved.

#include "LinterManager.h"
#include "Engine/World.h"

void FLinterManager::RegisterLinters()
{
	DefaultLinter = MakeShareable(new FLinterBase());

	// @TODO: Define Macro that works on Linux/Mac. This works on MSVC only.
	//#define REGISTER_LINTER(UCLASSTYPE) Linters.Add(U##UCLASSTYPE##::StaticClass(), MakeShareable(new F##UCLASSTYPE##Linter()));

	Linters.Add(UBlueprint::StaticClass(), MakeShareable(new FBlueprintLinter()));
	Linters.Add(UWorld::StaticClass(), MakeShareable(new FWorldLinter()));
}

void FLinterManager::Lint(UObject* Object)
{
	if (Object != nullptr)
	{
		// Look for a linter matching this object's class. If not found, try default.
		TSharedPtr<FLinterBase>* Linter = Linters.Find(Object->GetClass());
		if (Linter == nullptr)
		{
			Linter = &DefaultLinter;
		}

		// Let the linter take over
		if (Linter != nullptr && (*Linter).IsValid())
		{
			// Add linter's report if it found something
			if ((*Linter)->Lint(Object))
			{
				ErrorCount += (*Linter)->GetErrorCount();
				JSONReport += (*Linter)->GetAndClearReport() + TEXT(",");
			}
		}
	}
}

void FLinterManager::StartReport()
{
	JSONReport = TEXT("[");
	ErrorCount = 0;
}


void FLinterManager::EndReport()
{
	JSONReport.RemoveFromEnd(TEXT(","));
	JSONReport += TEXT("]");
}

uint32 FLinterManager::GetErrorCount()
{
	return ErrorCount;
}

FString FLinterManager::GetAndClearReport()
{
	FString Report = JSONReport;
	JSONReport.Empty();
	ErrorCount = 0;
	return Report;
}

FLinterManager::FLinterManager()
{
	RegisterLinters();
	ErrorCount = 0;
}
