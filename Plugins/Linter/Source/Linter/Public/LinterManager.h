// Copyright 2016 Gamemakin LLC. All Rights Reserved.

#pragma once

#include "Linters/LinterBase.h"
#include "Linters/BlueprintLinter.h"
#include "Linters/WorldLinter.h"

class FLinterManager
{
public:
	FLinterManager();
	void RegisterLinters();

	void Lint(UObject* Object);

	void StartReport();
	void EndReport();
	uint32 GetErrorCount();
	FString GetAndClearReport();
private:
	TMap<class UClass*, TSharedPtr<FLinterBase>> Linters;
	TSharedPtr<FLinterBase> DefaultLinter;
	FString JSONReport;
	uint32 ErrorCount;
};