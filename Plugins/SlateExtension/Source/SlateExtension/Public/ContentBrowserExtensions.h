// Copyright 2016 Gamemakin LLC. All Rights Reserved.

#pragma once

// Integrate Linter actions into the Content Browser context menu
class FSlateExtensionContentBrowserExtensions
{
public:
	static void InstallHooks();
	static void RemoveHooks();
};