// Copyright 2016 Gamemakin LLC. All Rights Reserved.

#include "Linters/WorldLinter.h"
#include "LintMessages.h"

bool FWorldLinter::HasAGoodPath()
{
	if (!GetDefault<ULinterSettings>()->bDoFolderNameChecks)
	{
		return true;
	}

	if (FLinterBase::HasAGoodPath())
	{
		if (!GetDefault<ULinterSettings>()->bMapsInMapsFolder)
		{
			return true;
		}

		FString PathName = Object->GetPathName();
		if (PathName.Contains(TEXT("/Maps/"), ESearchCase::CaseSensitive))
		{
			return true;
		}
		
		return ReportError(*Object->GetName(), Rule_2_4);
	}
	
	return false;
}
