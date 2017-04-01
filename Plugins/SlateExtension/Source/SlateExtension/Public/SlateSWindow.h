// Copyright 2016 Gamemakin LLC. All Rights Reserved.

#pragma once

#include "EditorStyleSet.h"
#include "SEditableTextBox.h"
#include "SCheckBox.h"
#include "SWindow.h"
#include "SUserWidget.h"
#include "SCompoundWidget.h"
#include "SUniformGridPanel.h"
#include "SSeparator.h"

#define LOCTEXT_NAMESPACE "SlateSWindow"

/**
* FDlgBatchRenameTool
*
* Wrapper class for SDlgBatchRenameTool. This class creates and launches a dialog then awaits the
* result to return to the user.
*/
class FDlgShowErrorList
{
public:
	enum EResult
	{
		Cancel = 0,			// No/Cancel, normal usage would stop the current action
		Confirm = 1,		// Yes/Ok/Etc, normal usage would continue with action
	};
	FDlgShowErrorList();
//	FDlgShowErrorList(const TArray<TSharedPtr<class SDlgSlateSWindow>>& InErrorMessages);

	/**  Shows the dialog box and waits for the user to respond. */
	EResult ShowModal();

	FString Prefix;
	FString Suffix;
	bool bRemovePrefix;
	bool bRemoveSuffix;

	FString Find;
	FString Replace;

	TArray<TSharedPtr<class FErrorList>>& ErrorMessages;
private:

	/** Cached pointer to the modal window */
	TSharedPtr<SWindow> DialogWindow;

	/** Cached pointer to the batch rename tool widget */
	TSharedPtr<class SDlgSlateSWindow> DialogWidget;
};

/**
* Slate panel for batch renaming
*/
class SDlgSlateSWindow : public SCompoundWidget
{
public:

	SLATE_BEGIN_ARGS(SDlgSlateSWindow)
		{}
		/** Window in which this widget resides */
		SLATE_ATTRIBUTE(TSharedPtr<SWindow>, ParentWindow)
	SLATE_END_ARGS()

	/**
	* Constructs this widget
	*
	* @param	InArgs	The declaration data for this widget
	*/
	void Construct(const FArguments& InArgs);

	/**
	* Returns the EResult of the button which the user pressed. Closing of the dialog
	* in any other way than clicking "Ok" results in this returning a "Cancel" value
	*/
	FDlgShowErrorList::EResult GetUserResponse() const;

private:

	/**
	* Handles when a button is pressed, should be bound with appropriate EResult Key
	*
	* @param ButtonID - The return type of the button which has been pressed.
	*/
	FReply OnButtonClick(FDlgShowErrorList::EResult ButtonID)
	{
		ParentWindow->RequestDestroyWindow();
		UserResponse = ButtonID;

		return FReply::Handled();
	}

	/** Stores the users response to this dialog */
	FDlgShowErrorList::EResult UserResponse;

	/** Pointer to the window which holds this Widget, required for modal control */
	TSharedPtr<SWindow>	ParentWindow;

public:

	TSharedPtr<SEditableTextBox> PrefixTextBox;
	TSharedPtr<SEditableTextBox> SuffixTextBox;
	TSharedPtr<SEditableTextBox> FindTextBox;
	TSharedPtr<SEditableTextBox> ReplaceTextBox;
	TSharedPtr<SCheckBox> PrefixRemoveBox;
	TSharedPtr<SCheckBox> SuffixRemoveBox;
};

#undef LOCTEXT_NAMESPACE