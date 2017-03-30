// Copyright 2016 Gamemakin LLC. All Rights Reserved.

#include "ContentBrowserExtensions.h"

#include "ContentBrowserModule.h"
#include "AssetToolsModule.h"
#include "AssetRegistryModule.h"
#include "IAssetTools.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"



#define LOCTEXT_NAMESPACE "SlateExtension"

DEFINE_LOG_CATEGORY_STATIC(SlateExtensionCBExtensions, Log, All);

//////////////////////////////////////////////////////////////////////////

// DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<FExtender>, FContentBrowserMenuExtender_SelectedPaths, const TArray<FString>& /*SelectedPaths*/);
FContentBrowserMenuExtender_SelectedPaths SlateExtensionContentBrowserPathsExtenderDelegate;
FDelegateHandle SlateExtensionContentBrowserPathsExtenderDelegateHandle;
// DECLARE_DELEGATE_RetVal_OneParam(TSharedRef<FExtender>, FContentBrowserMenuExtender_SelectedAssets, const TArray<FAssetData>& /*SelectedAssets*/);
FContentBrowserMenuExtender_SelectedAssets SlateExtensionContentBrowserAssetsExtenderDelegate;
FDelegateHandle SlateExtensionContentBrowserAssetsExtenderDelegateHandle;


//////////////////////////////////////////////////////////////////////////
// FLinterContentBrowserSelectedPathsExtensionBase

struct FSlateExtensionContentBrowserSelectedPathsExtensionBase
{
public:
	TArray<FString> SelectedPaths;

public:
	virtual void Execute() {}
	virtual ~FSlateExtensionContentBrowserSelectedPathsExtensionBase() {}
};

//////////////////////////////////////////////////////////////////////////
// FRunLinterForPathExtension

struct FRunSlateExtensionForPathExtension : public FSlateExtensionContentBrowserSelectedPathsExtensionBase
{
	virtual void Execute() override
	{
		UE_LOG(SlateExtensionCBExtensions, Display, TEXT("Starting SlateExtension."));

/*		FLinterCommandletManager* LinterCommandletManager = FModuleManager::LoadModuleChecked<FLinterModule>(TEXT("Linter")).GetLinterCommandletManager();
		if (LinterCommandletManager && LinterCommandletManager->RunLinter(SelectedPaths))
		{
			UE_LOG(LinterCBExtensions, Display, TEXT("Linter started."));
		}
		else
		{
			UE_LOG(LinterCBExtensions, Error, TEXT("Failed to start Linter. Already Linting?"));
		}*/
	}
};

//////////////////////////////////////////////////////////////////////////
// FLinterContentBrowserSelectedAssetsExtensionBase

struct FSlateExtensionContentBrowserSelectedAssetsExtensionBase
{
public:
	TArray<FAssetData> SelectedAssets;

public:
	virtual void Execute() {}
	virtual ~FSlateExtensionContentBrowserSelectedAssetsExtensionBase() {}
};

//////////////////////////////////////////////////////////////////////////
// FBatchRenameAssetsExtension

struct FBatchRenameAssetsExtension : public FSlateExtensionContentBrowserSelectedAssetsExtensionBase
{
	virtual void Execute() override
	{
		UE_LOG(SlateExtensionCBExtensions, Display, TEXT("Starting batch rename."));
/*		FDlgBatchRenameTool AssetDlg;
		if (AssetDlg.ShowModal() == FDlgBatchRenameTool::Confirm)
		{
			FAssetToolsModule& AssetToolsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
			TArray<FAssetRenameData> AssetsAndNames;

			for (auto AssetIt = SelectedAssets.CreateConstIterator(); AssetIt; ++AssetIt)
			{
				const FAssetData& Asset = *AssetIt;

				// Early out on assets that can not be renamed
				if (!(!Asset.IsRedirector() && Asset.AssetClass != NAME_Class && !(Asset.PackageFlags & PKG_FilterEditorOnly)))
				{
					continue;
				}

				// Work on a copy of the asset name and see if after name operations
				// if the copy is different than the original before creating rename data
				FString AssetNewName = Asset.AssetName.ToString();

				if (!AssetDlg.Find.IsEmpty())
				{
					AssetNewName.ReplaceInline(*AssetDlg.Find, *AssetDlg.Replace);
				}

				if (!AssetDlg.Prefix.IsEmpty())
				{
					if (AssetDlg.bRemovePrefix)
					{
						AssetNewName.RemoveFromStart(AssetDlg.Prefix, ESearchCase::CaseSensitive);
					}
					else
					{
						if (!AssetNewName.StartsWith(AssetDlg.Prefix, ESearchCase::CaseSensitive))
						{
							AssetNewName.InsertAt(0, AssetDlg.Prefix);
						}
					}
				}

				if (!AssetDlg.Suffix.IsEmpty())
				{
					if (AssetDlg.bRemoveSuffix)
					{
						AssetNewName.RemoveFromEnd(AssetDlg.Suffix, ESearchCase::CaseSensitive);
					}
					else
					{
						if (!AssetNewName.EndsWith(AssetDlg.Suffix, ESearchCase::CaseSensitive))
						{
							AssetNewName = AssetNewName.Append(AssetDlg.Suffix);
						}
					}
				}

				if (AssetNewName != Asset.AssetName.ToString())
				{
					AssetsAndNames.Push(FAssetRenameData(Asset.GetAsset(), Asset.PackagePath.ToString(), AssetNewName));
				}
			}

			AssetToolsModule.Get().RenameAssets(AssetsAndNames);
		}*/
	}
};

//////////////////////////////////////////////////////////////////////////
// FPaperContentBrowserExtensions_Impl

class FSlateExtensionContentBrowserExtensions_Impl
{
public:

	/////////////////////////////////////////////////////////////////////
	// Selected Path extension

	static void ExecuteSelectedPathFunctor(TSharedPtr<FSlateExtensionContentBrowserSelectedPathsExtensionBase> SelectedPathsFunctor)
	{
		SelectedPathsFunctor->Execute();
	}

	static void CreateSlateExtensionPathActionsSubMenu(FMenuBuilder& MenuBuilder, TArray<FString> SelectedPaths)
	{
/*		MenuBuilder.AddSubMenu(
			LOCTEXT("LinterPathActionsSubMenuLabel", "Linter Actions"),
			LOCTEXT("LinterPathActionsSubMenuToolTip", "Linter-related actions for this path."),
			FNewMenuDelegate::CreateStatic(&FSlateExtensionContentBrowserExtensions_Impl::PopulatePathLinterActionsMenu, SelectedPaths),
			false,
			FSlateIcon()
			);
			*/
	}

	static void PopulatePathSlateExtensionActionsMenu(FMenuBuilder& MenuBuilder, TArray<FString> SelectedPaths)
	{
		// Create sprites
		/*
		TSharedPtr<FRunLinterForPathExtension> RunLinterFunctor = MakeShareable(new FRunLinterForPathExtension());
		RunLinterFunctor->SelectedPaths = SelectedPaths;

		FUIAction Action_RunLinter(
			FExecuteAction::CreateStatic(&FLinterContentBrowserExtensions_Impl::ExecuteSelectedPathFunctor, StaticCastSharedPtr<FLinterContentBrowserSelectedPathsExtensionBase>(RunLinterFunctor)));

	    const FName LinterStyleSetName = FLinterStyle::Get()->GetStyleSetName();

		MenuBuilder.AddMenuEntry(
			LOCTEXT("CB_Extension_RunLinter", "Run Linter"),
			LOCTEXT("CB_Extension_RunLinter_Tooltip", "Run Linter on selected paths."),
			FSlateIcon(LinterStyleSetName, "AssetActions.RunLinter"),
			Action_RunLinter,
			NAME_None,
			EUserInterfaceActionType::Button);
			*/
	}

	static TSharedRef<FExtender> OnExtendContentBrowserPathSelectionMenu(const TArray<FString>& SelectedPaths)
	{
		
		TSharedRef<FExtender> Extender(new FExtender());
		/*
		// Run thru the paths to determine if any meet our criteria
		bool bAnyContentPaths = false;
		for (auto PathIt = SelectedPaths.CreateConstIterator(); PathIt; ++PathIt)
		{
			const FString& Asset = *PathIt;
			bAnyContentPaths = bAnyContentPaths || Asset.StartsWith(TEXT("/Game"));
		}

		if (bAnyContentPaths)
		{
			// Add the sprite actions sub-menu extender
			Extender->AddMenuExtension(
				"PathViewFolderOptions",
				EExtensionHook::After,
				nullptr,
				FMenuExtensionDelegate::CreateStatic(&FLinterContentBrowserExtensions_Impl::CreateLinterPathActionsSubMenu, SelectedPaths));
		}*/

		return Extender;
	}

	static TArray<FContentBrowserMenuExtender_SelectedPaths>& GetPathExtenderDelegates()
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
		return ContentBrowserModule.GetAllPathViewContextMenuExtenders();
	}

	/////////////////////////////////////////////////////////////////////
	// Selected Assets extension

	static void ExecuteSelectedAssetsFunctor(TSharedPtr<FSlateExtensionContentBrowserSelectedAssetsExtensionBase> SelectedAssetsFunctor)
	{
		SelectedAssetsFunctor->Execute();
	}

	static void CreateSlateExtensionAssetActionsSubMenu(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets)
	{
		/*MenuBuilder.AddSubMenu(
			LOCTEXT("LinterAssetActionsSubMenuLabel", "Linter Actions"),
			LOCTEXT("LinterAssetActionsSubMenuToolTip", "Linter-related actions for selected assets."),
			FNewMenuDelegate::CreateStatic(&FLinterContentBrowserExtensions_Impl::PopulateAssetLinterActionsMenu, SelectedAssets),
			false,
			FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetActions")
			);
			*/
	}

	static void PopulateAssetSlateExtensionActionsMenu(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets)
	{
		// Create sprites
		/*TSharedPtr<FBatchRenameAssetsExtension> BatchRenameFunctor = MakeShareable(new FBatchRenameAssetsExtension());
		BatchRenameFunctor->SelectedAssets = SelectedAssets;

		FUIAction Action_BatchRename(
			FExecuteAction::CreateStatic(&FLinterContentBrowserExtensions_Impl::ExecuteSelectedAssetsFunctor, StaticCastSharedPtr<FLinterContentBrowserSelectedAssetsExtensionBase>(BatchRenameFunctor)));

		const FName LinterStyleSetName = FLinterStyle::Get()->GetStyleSetName();

		MenuBuilder.AddMenuEntry(
			LOCTEXT("CB_Extension_BatchRename", "Batch Rename (Dangerous)..."),
			LOCTEXT("CB_Extension_BatchRename_Tooltip", "Batch rename selected assets. This is a potentially dangerous operation. Please back up your project first."),
			FSlateIcon(LinterStyleSetName, "AssetActions.BatchRename"),
			Action_BatchRename,
			NAME_None,
			EUserInterfaceActionType::Button);
			*/
	}

	static TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets)
	{
		TSharedRef<FExtender> Extender(new FExtender());

		// Run thru the assets to determine if any meet our criteria
		/*bool bAnyAssetCanBeRenamed = false;
		for (auto AssetIt = SelectedAssets.CreateConstIterator(); AssetIt; ++AssetIt)
		{
			const FAssetData& Asset = *AssetIt;
			// Cannot rename redirectors or classes or cooked packages
			if (!Asset.IsRedirector() && Asset.AssetClass != NAME_Class && !(Asset.PackageFlags & PKG_FilterEditorOnly))
			{
				bAnyAssetCanBeRenamed = true;
				break;
			}
		}

		if (bAnyAssetCanBeRenamed)
		{
			// Add the sprite actions sub-menu extender
			Extender->AddMenuExtension(
				"CommonAssetActions",
				EExtensionHook::After,
				nullptr,
				FMenuExtensionDelegate::CreateStatic(&FLinterContentBrowserExtensions_Impl::CreateLinterAssetActionsSubMenu, SelectedAssets));
		}
		*/
		return Extender;
	}

	static TArray<FContentBrowserMenuExtender_SelectedAssets>& GetAssetExtenderDelegates()
	{
		FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>(TEXT("ContentBrowser"));
		return ContentBrowserModule.GetAllAssetViewContextMenuExtenders();
	}
};

//////////////////////////////////////////////////////////////////////////
// FLinterContentBrowserExtensions

void FSlateExtensionContentBrowserExtensions::InstallHooks()
{

	SlateExtensionContentBrowserPathsExtenderDelegate = FContentBrowserMenuExtender_SelectedPaths::CreateStatic(&FSlateExtensionContentBrowserExtensions_Impl::OnExtendContentBrowserPathSelectionMenu);
	TArray<FContentBrowserMenuExtender_SelectedPaths>& CBMenuPathExtenderDelegates = FSlateExtensionContentBrowserExtensions_Impl::GetPathExtenderDelegates();
	CBMenuPathExtenderDelegates.Add(SlateExtensionContentBrowserPathsExtenderDelegate);
	SlateExtensionContentBrowserPathsExtenderDelegateHandle = SlateExtensionContentBrowserPathsExtenderDelegate.GetHandle();

	SlateExtensionContentBrowserAssetsExtenderDelegate = FContentBrowserMenuExtender_SelectedAssets::CreateStatic(&FSlateExtensionContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu);
	TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuAssetsExtenderDelegates= FSlateExtensionContentBrowserExtensions_Impl::GetAssetExtenderDelegates();
	CBMenuAssetsExtenderDelegates.Add(SlateExtensionContentBrowserAssetsExtenderDelegate);
	SlateExtensionContentBrowserAssetsExtenderDelegateHandle = SlateExtensionContentBrowserAssetsExtenderDelegate.GetHandle();
	
	
	/*LinterContentBrowserPathsExtenderDelegate = FContentBrowserMenuExtender_SelectedPaths::CreateStatic(&FLinterContentBrowserExtensions_Impl::OnExtendContentBrowserPathSelectionMenu);
	TArray<FContentBrowserMenuExtender_SelectedPaths>& CBMenuPathExtenderDelegates = FLinterContentBrowserExtensions_Impl::GetPathExtenderDelegates();
	CBMenuPathExtenderDelegates.Add(LinterContentBrowserPathsExtenderDelegate);
	LinterContentBrowserPathsExtenderDelegateHandle = CBMenuPathExtenderDelegates.Last().GetHandle();

	LinterContentBrowserAssetsExtenderDelegate = FContentBrowserMenuExtender_SelectedAssets::CreateStatic(&FLinterContentBrowserExtensions_Impl::OnExtendContentBrowserAssetSelectionMenu);
	TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuAssetExtenderDelegates = FLinterContentBrowserExtensions_Impl::GetAssetExtenderDelegates();
	CBMenuAssetExtenderDelegates.Add(LinterContentBrowserAssetsExtenderDelegate);
	LinterContentBrowserAssetsExtenderDelegateHandle = CBMenuAssetExtenderDelegates.Last().GetHandle();
	*/
}

void FSlateExtensionContentBrowserExtensions::RemoveHooks()
{
	/*
	TArray<FContentBrowserMenuExtender_SelectedPaths>& CBMenuPathExtenderDelegates = FLinterContentBrowserExtensions_Impl::GetPathExtenderDelegates();
	CBMenuPathExtenderDelegates.RemoveAll([](const FContentBrowserMenuExtender_SelectedPaths& Delegate) { return Delegate.GetHandle() == LinterContentBrowserPathsExtenderDelegateHandle; });

	TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuAssetExtenderDelegates = FLinterContentBrowserExtensions_Impl::GetAssetExtenderDelegates();
	CBMenuAssetExtenderDelegates.RemoveAll([](const FContentBrowserMenuExtender_SelectedAssets& Delegate) { return Delegate.GetHandle() == LinterContentBrowserAssetsExtenderDelegateHandle; });
    */

	TArray<FContentBrowserMenuExtender_SelectedPaths>& CBMenuPathExtenderDelegates = FSlateExtensionContentBrowserExtensions_Impl::GetPathExtenderDelegates();
	CBMenuPathExtenderDelegates.RemoveAll(
		[](const FContentBrowserMenuExtender_SelectedPaths& Delegate) {
		return Delegate.GetHandle() == SlateExtensionContentBrowserPathsExtenderDelegateHandle;
	});

	TArray<FContentBrowserMenuExtender_SelectedAssets>& CBMenuAssetsExtenderDelegates = FSlateExtensionContentBrowserExtensions_Impl::GetAssetExtenderDelegates();
	CBMenuAssetsExtenderDelegates.RemoveAll(
		[](const FContentBrowserMenuExtender_SelectedAssets& Delegate) {
		return Delegate.GetHandle() == SlateExtensionContentBrowserAssetsExtenderDelegateHandle;
	});
}

//////////////////////////////////////////////////////////////////////////

#undef LOCTEXT_NAMESPACE