// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "EditorStyle.h"
#include "Framework/Commands/Commands.h"
//#include "PaperStyle.h"

class FSlateExtensionEditorCommands : public TCommands<FSlateExtensionEditorCommands>
{
public:
	FSlateExtensionEditorCommands()
		: TCommands<FSlateExtensionEditorCommands>(
			TEXT("SlateExtensionEditor"), // Context name for fast lookup
			NSLOCTEXT("Contexts", "SlateExtensionEditor", "SlateExtension Editor"), // Localized context name for displaying
			NAME_None,
			TEXT("SlatExtensionStyleName")
			)
	{
	}

	// TCommand<> interface
	virtual void RegisterCommands() override;
	// End of TCommand<> interface

public:
	
	TSharedPtr<FUICommandInfo> SlateButton;

	/*TSharedPtr<FUICommandInfo> SetShowGrid;
	TSharedPtr<FUICommandInfo> SetShowBounds;
	TSharedPtr<FUICommandInfo> SetShowCollision;

	// View Menu Commands
	TSharedPtr<FUICommandInfo> SetShowPivot;
	TSharedPtr<FUICommandInfo> SetShowSockets;

	// Timeline commands
	TSharedPtr<FUICommandInfo> AddNewFrame;
	TSharedPtr<FUICommandInfo> AddNewFrameBefore;
	TSharedPtr<FUICommandInfo> AddNewFrameAfter;

	// Asset commands
	TSharedPtr<FUICommandInfo> PickNewSpriteFrame;
	TSharedPtr<FUICommandInfo> EditSpriteFrame;
	TSharedPtr<FUICommandInfo> ShowInContentBrowser;
	*/
};
