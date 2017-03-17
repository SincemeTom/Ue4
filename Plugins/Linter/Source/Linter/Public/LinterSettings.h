// Copyright 2016 Gamemakin LLC. All Rights Reserved.

#pragma once
#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "LinterSettings.generated.h"


/**
 * Class/Prefix/Suffix settings for Linter
 */
USTRUCT()
struct FAssetTypeNameSetting
{
	GENERATED_USTRUCT_BODY()

	FAssetTypeNameSetting()
	{}

	FAssetTypeNameSetting(FString InPrefix, FString InSuffix, FName InSpecialCase = FName(NAME_None))
		: Prefix(InPrefix)
		, Suffix(InSuffix)
		, SpecialCase(InSpecialCase)
	{}

	UPROPERTY(EditAnywhere, Category = Default)
	FString Prefix;

	UPROPERTY(EditAnywhere, Category = Default)
	FString Suffix;

	UPROPERTY(VisibleAnywhere, Category = Default)
	FName SpecialCase;
};

/**
* Implements the settings for the Linter plugin.
*/
UCLASS(config = Linter, defaultconfig)
class ULinterSettings : public UObject
{
	GENERATED_UCLASS_BODY()

	// Enables configuration of prefixes and suffixes for linting
	UPROPERTY(EditAnywhere, config, Category = Settings)
	TMap<UClass*, FAssetTypeNameSetting> AssetNameSettings;

	UPROPERTY(EditAnywhere, config, Category = Settings)
	FAssetTypeNameSetting MacroLibrarySetting;

	// Section 1 - Asset Names

	UPROPERTY(EditAnywhere, config, Category = "Checks | 1. Asset Naming Conventions")
	bool bDoAssetNamingConventionChecks;

	// Section 2 - Structure

	UPROPERTY(EditAnywhere, config, Category = "Checks | 2. Content Directory Structure", meta = (DisplayName = "2.1 Do Folder Name Checks"))
	bool bDoFolderNameChecks;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 2. Content Directory Structure", meta = (DisplayName = "2.1.1 Always Use PascalCase", EditCondition = "bDoFolderNameChecks"))
	bool bDoFolderNamePascalCaseCheck;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 2. Content Directory Structure", meta = (DisplayName = "2.1.2 Never Use Spaces", EditCondition="bDoFolderNameChecks"))
	bool bDoFolderNameSpaceCheck;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 2. Content Directory Structure", meta = (DisplayName = "2.1.3 Never Use Unicode Characters And Other Symbols", EditCondition = "bDoFolderNameChecks"))
	bool bDoFolderNameUnicodeCheck;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 2. Content Directory Structure", meta = (DisplayName = "2.2 Use A Top Level Folder For Project Specific Assets", EditCondition = "bDoFolderNameChecks"))
	bool bUseTopLevelFolderForAssets;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 2. Content Directory Structure", meta = (DisplayName = "2.4 All Map Files Belong In A Folder Called Maps", EditCondition = "bDoFolderNameChecks"))
	bool bMapsInMapsFolder;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 2. Content Directory Structure", meta = (DisplayName = "2.6.1 Creating a folder named Assets is redundant.", EditCondition = "bDoFolderNameChecks"))
	bool bDoFolderNameAssetsRedundantCheck;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 2. Content Directory Structure", meta = (DisplayName = "2.6.2 Creating a folder named Meshes, Textures, or Materials is redundant.", EditCondition = "bDoFolderNameChecks"))
	bool bDoFolderNameAssetTypeRedundantCheck;

	// Section 3 - Blueprints

	UPROPERTY(EditAnywhere, config, Category = "Checks | 3. Blueprints", meta = (DisplayName = "3.1 Compiles Successfully"))
	bool bBPCompiles;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 3. Blueprints", meta = (DisplayName = "3.2 Do Variable Checks"))
	bool bBPVariableChecks;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 3. Blueprints", meta = (DisplayName = "3.2.1.2 Pascal Case", EditCondition = "bBPVariableChecks"))
	bool bBPVarPascalCase;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 3. Blueprints", meta = (DisplayName = "3.2.1.3 Boolean 'b' Prefix", EditCondition = "bBPVariableChecks"))
	bool bBPVarBoolPrefix;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 3. Blueprints", meta = (DisplayName = "3.2.1.4.1 Boolean 'Is' Check", EditCondition = "bBPVariableChecks"))
	bool bBPVarBoolIsQuestion;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 3. Blueprints", meta = (DisplayName = "3.2.1.6 Do Not Include Atomic Type Names", EditCondition = "bBPVariableChecks"))
	bool bBPVarAtomicInclusion;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 3. Blueprints", meta = (DisplayName = "3.2.1.8 Arrays Should Be Pural Nouns", EditCondition = "bBPVariableChecks"))
	bool bBPVarArrayPlurals;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 3. Blueprints", meta = (DisplayName = "3.2.2 Expose On Spawn Must Be Editable", EditCondition = "bBPVariableChecks"))
	bool bBPVarExposeOnSpawnEditable;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 3. Blueprints", meta = (DisplayName = "3.3 Categorize Editable Variables In 5+ Var Classes", EditCondition = "bBPVariableChecks"))
	bool bBPVarEditableCategories;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 3. Blueprints", meta = (DisplayName = "3.2.7 SaveGame Variables Must Be In A SaveGame", EditCondition = "bBPVariableChecks"))
	bool bBPVarSaveGameInSaveGame;

	UPROPERTY(EditAnywhere, config, Category = "Checks | 3. Blueprints", meta = (DisplayName = "3.2.8 No Config Variables", EditCondition = "bBPVariableChecks"))
	bool bBPVarSaveNoConfig;

};
