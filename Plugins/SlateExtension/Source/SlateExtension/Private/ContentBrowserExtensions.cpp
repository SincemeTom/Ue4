// Copyright 2016 Gamemakin LLC. All Rights Reserved.

#include "ContentBrowserExtensions.h"
#include "ContentBrowserModule.h"
#include "AssetToolsModule.h"
#include "AssetRegistryModule.h"
#include "IAssetTools.h"
#include "EditorStyle.h"
#include "EngineUtils.h"
#include "Engine/Blueprint.h"
#include "SlateExtension.h"
#include "SlateSWindow.h"
#include "SlateWindowHelper.h"
#include "SlateExtensionEditorCommands.h"
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


struct FBlueprintErrorMessage
{
public:
	FBlueprintErrorMessage(EBlueprintStatus InErrorType, const FString& InErrorMessage)
		:ErrorType(InErrorType),
		ErrorMessage(InErrorMessage)
	{
	}
	void operator=(FBlueprintErrorMessage Other)
	{
		ErrorType = Other.ErrorType;
		ErrorMessage = Other.ErrorMessage;
	}
public:
	EBlueprintStatus ErrorType;

	FString ErrorMessage;
};

static bool IsCompiledCorrectly(UBlueprint* Blueprint, TSharedPtr<FBlueprintErrorMessage>& OutError)
{
	check(Blueprint);
	FString ErrorString;
	switch (Blueprint->Status)
	{
	case EBlueprintStatus::BS_BeingCreated:
	case EBlueprintStatus::BS_Dirty:
	case EBlueprintStatus::BS_Unknown:
	case EBlueprintStatus::BS_UpToDate:
		return true;
	case EBlueprintStatus::BS_Error:

		ErrorString = FString::Printf(TEXT("%s is failing to compile due to errors. Path is %s"), *Blueprint->GetName(), *Blueprint->GetPathName());
//		UE_LOG(LogSlateExtension, Error, TEXT("%s"), *ErrorString)	
		OutError = MakeShareable(new FBlueprintErrorMessage(Blueprint->Status, ErrorString));
		return false;
	case EBlueprintStatus::BS_UpToDateWithWarnings:

		ErrorString = FString::Printf(TEXT("%s is compiling but has warnings. Path is %s"), *Blueprint->GetName(), *Blueprint->GetPathName());
//		UE_LOG(LogSlateExtension, Error, TEXT("%s"), *ErrorString)
		OutError = MakeShareable(new FBlueprintErrorMessage(Blueprint->Status, ErrorString));
		return false;
	default:
		return true;
	}
}
static void PrintErrorListMessage(const TArray<TSharedPtr<FBlueprintErrorMessage>>& ErrorList)
{
	UE_LOG(LogSlateExtension, Log, TEXT("-------------------------------------------------ErrorList-----------------------------------"))

		for (TSharedPtr<FBlueprintErrorMessage> message : ErrorList)
		{
			switch (message->ErrorType)
			{
			case EBlueprintStatus::BS_Error:
				UE_LOG(LogSlateExtension, Error, TEXT("%s"), *message->ErrorMessage)
					break;
			case EBlueprintStatus::BS_UpToDateWithWarnings:
				UE_LOG(LogSlateExtension, Warning, TEXT("%s"), *message->ErrorMessage)
					break;
			default:
				break;
			}

		}
	
	TSharedPtr<FDlgShowErrorList> AssetDlg = MakeShareable(new FDlgShowErrorList(ErrorList));
	if (AssetDlg->ShowModal() == FDlgShowErrorList::Confirm)
	{

	}
}
static void CheckSelectedAssets(const TArray<FAssetData>& SelectedAssets)
{
	UE_LOG(LogSlateExtension, Log, TEXT("------------------------------------------Start Check Selected Assets-----------------------------------"))
	TArray<TSharedPtr<FBlueprintErrorMessage>> ErrorList;
	for (FAssetData Asset : SelectedAssets)
	{
		UObject* LoadedObject = Asset.GetAsset();
		if (LoadedObject != nullptr)
		{
			UE_LOG(LogSlateExtension, Log, TEXT("Loaded '%s'"), *Asset.ObjectPath.ToString())
				UBlueprint*	Blueprint = Cast<UBlueprint>(LoadedObject);
			TSharedPtr<FBlueprintErrorMessage> ErrorMessage;
			if (Blueprint && !IsCompiledCorrectly(Blueprint, ErrorMessage))
			{
				ErrorList.Add(ErrorMessage);
			}

		}
		else
		{
			UE_LOG(LogSlateExtension, Error, TEXT("Faild to load '%s'"), *Asset.ObjectPath.ToString())
		}
	}

	UE_LOG(LogSlateExtension, Log, TEXT("------------------------------------------End Check Selected Assets-----------------------------------"))

	PrintErrorListMessage(ErrorList);

}
static void CheckSelectedPaths(const TArray<FString>& SelectedPaths)
{
	UE_LOG(LogSlateExtension, Log, TEXT("------------------------------------------Start Check Selected Assets-----------------------------------"))
	
	TArray<TSharedPtr<FBlueprintErrorMessage>> ErrorList;
	for (const FString& Path : SelectedPaths)
	{
		TArray<UObject*> LoadedObjects;
		if (EngineUtils::FindOrLoadAssetsByPath(Path, LoadedObjects, EngineUtils::ATL_Regular))
		{
			for (UObject* LoadedObject : LoadedObjects)
			{
				if (LoadedObject)
				{
					UE_LOG(LogSlateExtension, Log, TEXT("Loaded '%s'"), *LoadedObject->GetPathName())
					UBlueprint* Blueprint = Cast<UBlueprint>(LoadedObject);
					TSharedPtr<FBlueprintErrorMessage> ErrorMessage;
					if (Blueprint && !IsCompiledCorrectly(Blueprint, ErrorMessage))
					{
						ErrorList.Add(ErrorMessage);
					}
				}
			}
			
		}

	}
	UE_LOG(LogSlateExtension, Log, TEXT("------------------------------------------End Check Selected Assets-----------------------------------"))

	PrintErrorListMessage(ErrorList);
}

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

		CheckSelectedPaths(SelectedPaths);
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

struct FMenuEntryAssetsExtension : public FSlateExtensionContentBrowserSelectedAssetsExtensionBase
{
	virtual void Execute() override
	{
		CheckSelectedAssets(SelectedAssets);

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
		MenuBuilder.BeginSection("Blueprint Checker", LOCTEXT("Blueprint Checker", "Blueprint Checker"));
		MenuBuilder.AddSubMenu(
			LOCTEXT("SlateExtensionAssetActionsSubMenuLabel", "Slate Extension Actions"),
			LOCTEXT("SlateExtensionAssetActionsSubMenuToolTip", "This is Slate Extension E.m.."),
			FNewMenuDelegate::CreateStatic(&FSlateExtensionContentBrowserExtensions_Impl::PopulatePathSlateExtensionActionsMenu, SelectedPaths),
			false,
			FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetActions")

		);
		MenuBuilder.EndSection();
	}

	static void PopulatePathSlateExtensionActionsMenu(FMenuBuilder& MenuBuilder, TArray<FString> SelectedPaths)
	{
	
		TSharedPtr<FRunSlateExtensionForPathExtension> RunSlateExtensionFunctor = MakeShareable(new FRunSlateExtensionForPathExtension());
		RunSlateExtensionFunctor->SelectedPaths = SelectedPaths;

		FUIAction Action_RunSlateExtension(
			FExecuteAction::CreateStatic(&FSlateExtensionContentBrowserExtensions_Impl::ExecuteSelectedPathFunctor, StaticCastSharedPtr<FSlateExtensionContentBrowserSelectedPathsExtensionBase>(RunSlateExtensionFunctor))
		);
		MenuBuilder.BeginSection("SubMenuHook", LOCTEXT("SlateExtensionCheckSelectedBlueprint", "CheckSelectedBlueprints"));
		MenuBuilder.AddMenuEntry(
			LOCTEXT("CB_Extension_RunLinter", "Check Blueprints"),
			LOCTEXT("CB_Extension_RunLinter_Tooltip", "Run Linter on selected paths."),
			FSlateIcon(FEditorStyle::GetStyleSetName(), "AssetActions.RunLinter"),
			Action_RunSlateExtension,
			NAME_None,
			EUserInterfaceActionType::Button);
		MenuBuilder.EndSection();
	}

	static TSharedRef<FExtender> OnExtendContentBrowserPathSelectionMenu(const TArray<FString>& SelectedPaths)
	{		
		TSharedRef<FExtender> Extender(new FExtender());
		Extender->AddMenuExtension(
			"PathViewFolderOptions",
			EExtensionHook::After,
			nullptr,
			FMenuExtensionDelegate::CreateStatic(&FSlateExtensionContentBrowserExtensions_Impl::CreateSlateExtensionPathActionsSubMenu, SelectedPaths)
			);

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
		MenuBuilder.BeginSection("Blueprint Checker", LOCTEXT("Blueprint Checker", "Blueprint Checker"));
		MenuBuilder.AddSubMenu(
			LOCTEXT("SlateExtensionAssetActionsSubMenuLabel", "Slate Extension Actions"),
			LOCTEXT("SlateExtensionAssetActionsSubMenuToolTip", "This is Slate Extension E.m.."),
			FNewMenuDelegate::CreateStatic(&FSlateExtensionContentBrowserExtensions_Impl::PopulateAssetSlateExtensionActionsMenu, SelectedAssets),
			false,
			FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetActions")

		);
		MenuBuilder.EndSection();

	}
	static void CreateSlateExtensionAssetActionsToolBar(FToolBarBuilder& ToolBarBuilder, TArray<FAssetData> SelectedAssets)
	{
		ToolBarBuilder.AddToolBarButton(
			FSlateExtensionEditorCommands::Get().SlateButton
		);
	}

	static void PopulateAssetSlateExtensionActionsMenu(FMenuBuilder& MenuBuilder, TArray<FAssetData> SelectedAssets)
	{
		TSharedPtr<FMenuEntryAssetsExtension> MenuEntryFunctor = MakeShareable(new FMenuEntryAssetsExtension);
		MenuEntryFunctor->SelectedAssets = SelectedAssets;
		FUIAction Action_MenuEntry(
			FExecuteAction::CreateStatic(&FSlateExtensionContentBrowserExtensions_Impl::ExecuteSelectedAssetsFunctor, StaticCastSharedPtr<FSlateExtensionContentBrowserSelectedAssetsExtensionBase>(MenuEntryFunctor))
		);
		MenuBuilder.BeginSection("SubMenuHook", LOCTEXT("SlateExtensionCheckSelectedBlueprint", "CheckSelectedBlueprints"));
		MenuBuilder.AddMenuEntry(
			LOCTEXT("SlateExtensionMenuEnry", "Check Blueprints"),
			LOCTEXT("SlateExtensionMenuEnryTooltip", "This is Slate Extension Menu Entry"),
			FSlateIcon(FEditorStyle::GetStyleSetName(), "ContentBrowser.AssetActions"),
			Action_MenuEntry,
			NAME_None,
			EUserInterfaceActionType::Button,
			NAME_None
			);
		MenuBuilder.EndSection();
	}

	static TSharedRef<FExtender> OnExtendContentBrowserAssetSelectionMenu(const TArray<FAssetData>& SelectedAssets)
	{
		TSharedRef<FExtender> Extender(new FExtender());
		
		Extender->AddMenuExtension(
			TEXT("CommonAssetActions"),
			EExtensionHook::After,
			nullptr,
			FMenuExtensionDelegate::CreateStatic(&FSlateExtensionContentBrowserExtensions_Impl::CreateSlateExtensionAssetActionsSubMenu, SelectedAssets)
		);
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
}

void FSlateExtensionContentBrowserExtensions::RemoveHooks()
{

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
//DEFINE_LOG_CATEGORY(LogSlateExtension);