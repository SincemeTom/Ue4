// Copyright 2016 Gamemakin LLC. All Rights Reserved.

#include "SlateSWindow.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Text/TextLayout.h"
#include "Framework/Docking/TabManager.h"
#include "ContentBrowserExtensions.h"
#include "SBox.h"
#include "STextBlock.h"
#include "SButton.h"
#include "SlateEnums.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "LinterBatchRenamer"

const FVector2D WindowSize = FVector2D(600, 900);

FDlgShowErrorList::FDlgShowErrorList()
	: bRemovePrefix(false)
	, bRemoveSuffix(false)
{
	if (FSlateApplication::IsInitialized())
	{
		DialogWindow = SNew(SWindow)
			.Title(LOCTEXT("BlueprintCheckerToolDlgTitle", "Blueprint Check Tool"))
			.SizingRule(ESizingRule::UserSized)
			.SupportsMinimize(false).SupportsMaximize(false)
			.SaneWindowPlacement(false)
			.AutoCenter(EAutoCenter::PreferredWorkArea)
			.ClientSize(WindowSize);


		TSharedPtr<SBorder> DialogWrapper =
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(4.0f)
			[
				SAssignNew(DialogWidget, SDlgSlateSWindow)
				.ParentWindow(DialogWindow)
			];

		DialogWindow->SetContent(DialogWrapper.ToSharedRef());
	}
}

FDlgShowErrorList::FDlgShowErrorList(const TArray<TSharedPtr<FBlueprintErrorMessage>>& InErrorMessages)
	:ErrorMessages(InErrorMessages)
	, bRemovePrefix(false)
	, bRemoveSuffix(false) 
{
	TSharedPtr<FDlgShowErrorList> TempPtr = MakeShareable(this);
	if (FSlateApplication::IsInitialized())
	{
		DialogWindow = SNew(SWindow)
			.Title(LOCTEXT("BlueprintCheckerToolDlgTitle", "Blueprint Check Tool"))
			.SupportsMinimize(false).SupportsMaximize(false)
			.SaneWindowPlacement(false)
			.AutoCenter(EAutoCenter::PreferredWorkArea)
			.ClientSize(WindowSize);

		TSharedPtr<SBorder> DialogWrapper =
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
			.Padding(4.0f)
			[
				SAssignNew(DialogWidget, SDlgSlateSWindow)
				.ParentWindow(DialogWindow)
				.ErrorListDlg(TempPtr)
			];

		DialogWindow->SetContent(DialogWrapper.ToSharedRef());

	}
}

FDlgShowErrorList::EResult FDlgShowErrorList::ShowModal()
{
	EResult UserResponse = (EResult)DialogWidget->GetUserResponse();

	TSharedPtr<SWindow> RootWindow = FGlobalTabmanager::Get()->GetRootWindow();
	if (RootWindow.IsValid())
	{
		FSlateApplication::Get().AddWindowAsNativeChild(DialogWindow.ToSharedRef(), RootWindow.ToSharedRef());
	}
	if (UserResponse == EResult::Confirm)
	{
		if (Prefix.IsEmpty() && Suffix.IsEmpty() && Find.IsEmpty())
		{
			return EResult::Cancel;
		}
	}
	return UserResponse;
}

void SDlgSlateSWindow::Construct(const FArguments& InArgs)
{
	UserResponse = FDlgShowErrorList::Cancel;
	ParentWindow = InArgs._ParentWindow.Get();
	ErrorListDlg = InArgs._ErrorListDlg.Get();

	this->ChildSlot[
		SNew(SVerticalBox)
			+ SVerticalBox::Slot()
			.AutoHeight()
			.Padding(8.0f,4.0f,8.0f,4.0f)
			[
				SAssignNew(ErrorListBox, SVerticalBox)
			]
			+ SVerticalBox::Slot()
			.AutoHeight()
			.HAlign(HAlign_Right)
			.Padding(8.0f, 4.0f, 8.0f, 4.0f)
			[
				SNew(SUniformGridPanel)
				.SlotPadding(FEditorStyle::GetMargin("StandardDialog.SlotPadding"))
				.MinDesiredSlotWidth(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
				.MinDesiredSlotHeight(FEditorStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
				+ SUniformGridPanel::Slot(0, 0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
					.OnClicked(this, &SDlgSlateSWindow::OnButtonClick, FDlgShowErrorList::Confirm)
					.Text(LOCTEXT("SkeletonMergeOk", "OK"))
				]
				+ SUniformGridPanel::Slot(1, 0)
				[
					SNew(SButton)
					.HAlign(HAlign_Center)
					.ContentPadding(FEditorStyle::GetMargin("StandardDialog.ContentPadding"))
					.OnClicked(this, &SDlgSlateSWindow::OnButtonClick, FDlgShowErrorList::Cancel)
					.Text(LOCTEXT("SkeletonMergeCancel", "Cancel"))
				]
			]
	];	
	ShowErrorList();
}
void SDlgSlateSWindow::ShowErrorList()
{
	if (ErrorListDlg.IsValid())
	{
		for (TSharedPtr<FBlueprintErrorMessage> message : ErrorListDlg->ErrorMessages)
		{
			ErrorListBox->AddSlot()
				.AutoHeight()
				.Padding(1.f)
				[
					SNew(SBorder)
					.BorderImage(FEditorStyle::GetBrush("ToolPanel.GroupBorder"))
					.OnMouseButtonDown(this, &SDlgSlateSWindow::OnBorderDown, message)
					.Padding(4.0f)
					[
						SNew(STextBlock)
						.Text(FText::FromString(message->ErrorMessage))
						.Font(FSlateFontInfo(TEXT("Robot"), 12))
						.ColorAndOpacity(FSlateColor(message->ErrorType == EBlueprintStatus::BS_Error ? FLinearColor::Red : FLinearColor::Yellow))
					]
				];
		}
	}
}
FDlgShowErrorList::EResult SDlgSlateSWindow::GetUserResponse() const
{
	return UserResponse;
}

FReply SDlgSlateSWindow::OnBorderDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent, TSharedPtr<struct FBlueprintErrorMessage> ErrorMessage)
{
	//UE_LOG(LogSlateExtension, TEXT("Press Button"))
	TArray<UObject*> Objects;
	Objects.Add(ErrorMessage.Get()->SelectedObject);
	GEditor->SyncBrowserToObjects(Objects);
	return FReply::Handled();
}
#undef LOCTEXT_NAMESPACE
