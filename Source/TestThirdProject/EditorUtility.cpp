// Fill out your copyright notice in the Description page of Project Settings.

#include "TestThirdProject.h"
#include "IContentBrowserSingleton.h"
#include "ContentBrowserModule.h"
#include "AssetData.h"
#include "EditorUtility.h"




TArray<class FAssetData> UEditorUtility::GetSelectionAssets()
{
	//@TODO: Blocking load, no slow dialog
	FContentBrowserModule& ContentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser");
	TArray<FAssetData> SelectedAssets;
	ContentBrowserModule.Get().GetSelectedAssets(SelectedAssets);
	return SelectedAssets;
}
void UEditorUtility::RenameSelectedAssets()
{
	TArray<FAssetData> SelectedAssets = GetSelectionAssets();

}
