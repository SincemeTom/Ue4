// Copyright 2016 Gamemakin LLC. All Rights Reserved.

#include "Linters/BlueprintLinter.h"
#include "Engine/Blueprint.h"
#include "LinterManager.h"
#include "EdGraphSchema_K2.h"
#include "BlueprintEditorUtils.h"
#include "LintMessages.h"
#include "GameFramework/SaveGame.h"

bool FBlueprintLinter::Lint(const UObject* InObject)
{
	START_LINT();

	Blueprint = CastChecked<UBlueprint>(Object);

	ClearBlueprintInfo();
	GatherBlueprintInfo();

	bFailedLint |= !PassesBasicAssetChecks();
	bFailedLint |= !IsCompiledCorrectly();
	bFailedLint |= !HasSaneVariables();
	bFailedLint |= !HasEditableVariablesCategorized();
	
	ClearBlueprintInfo();
	Blueprint = nullptr;
	
	END_LINT();
}

void FBlueprintLinter::ClearBlueprintInfo()
{
	EditableVariables.Reset();
	bIsMacroLibrary = false;
	bIsActor = false;
	CDO = nullptr;
	ACDO = nullptr;

	ComponentCount = 0;
	VariableCount = 0;
}

void FBlueprintLinter::GatherBlueprintInfo()
{
	check(Blueprint);

	bIsMacroLibrary = Blueprint->BlueprintType == EBlueprintType::BPTYPE_MacroLibrary;

	if (bIsMacroLibrary)
	{
		return;
	}

	CDO = Blueprint->GeneratedClass->GetDefaultObject();
	ACDO = Cast<AActor>(CDO);

	VariableCount = Blueprint->NewVariables.Num();

	for (FBPVariableDescription Desc : Blueprint->NewVariables)
	{
		if (FBlueprintEditorUtils::IsVariableComponent(Desc))
		{
			ComponentCount++;
			VariableCount--;
		}
		else
		{
			if ((Desc.PropertyFlags & CPF_DisableEditOnInstance) != CPF_DisableEditOnInstance)
			{
				EditableVariables.Add(Desc);
			}
		}
	}
}

bool FBlueprintLinter::IsNamedCorrectly()
{
	if (!GetDefault<ULinterSettings>()->bDoAssetNamingConventionChecks)
	{
		return true;
	}

	check(Blueprint);

	UClass* ObjectClass = Blueprint->GetClass();
	UClass* NativeBaseClass = nullptr;

	const FAssetTypeNameSetting* NameSetting = nullptr;

	// Macro libraries are a special case as they are always of base class UBlueprint
	if (bIsMacroLibrary)
	{
		NameSetting = &GetDefault<ULinterSettings>()->MacroLibrarySetting;
	}
	

	// If not a special case, see if there exists
	// a rule for the native class this Blueprint extends from...
	if (NameSetting == nullptr)
	{
		// Find the first native class this Blueprint represents in case
		// its native class has a special naming override.
		UClass* GeneratedClass = Blueprint->GeneratedClass;
		while (GeneratedClass != nullptr && !GeneratedClass->IsNative())
		{
			GeneratedClass = GeneratedClass->GetSuperClass();
		}
		NativeBaseClass = GeneratedClass;

		if (NativeBaseClass != nullptr)
		{
			NameSetting = GetDefault<ULinterSettings>()->AssetNameSettings.Find(NativeBaseClass);
		}
	}
	
	// ...otherwise stick to the object's base class.
	if (NameSetting == nullptr)
	{
		NameSetting = GetDefault<ULinterSettings>()->AssetNameSettings.Find(ObjectClass);
	}

	// Run prefix and suffix checks using found name settings if they are non-null
	if (NameSetting != nullptr)
	{
		if (!NameSetting->Prefix.IsEmpty())
		{
			if (!Object->GetName().StartsWith(NameSetting->Prefix, ESearchCase::CaseSensitive))
			{
				return ReportError(*Object->GetName(), FText::FormatNamed(Rule_1, TEXT("Correction"), FText::FromString(FString::Printf(TEXT("Should start with '%s'. "), *NameSetting->Prefix))));
			}
		}

		if (!NameSetting->Suffix.IsEmpty())
		{
			if (!Object->GetName().EndsWith(NameSetting->Suffix, ESearchCase::CaseSensitive))
			{
				return ReportError(*Object->GetName(), FText::FormatNamed(Rule_1, TEXT("Correction"), FText::FromString(FString::Printf(TEXT("Should end with '%s'. "), *NameSetting->Suffix))));
			}
		}
	}

	// If we don't have name settings or passed all name checks, simply return true
	return true;
}

bool FBlueprintLinter::IsCompiledCorrectly()
{
	if (!GetDefault<ULinterSettings>()->bBPCompiles)
	{
		return true;
	}

	check(Blueprint);

	switch (Blueprint->Status)
	{
		case EBlueprintStatus::BS_BeingCreated:
		case EBlueprintStatus::BS_Dirty:
		case EBlueprintStatus::BS_Unknown:
		case EBlueprintStatus::BS_UpToDate:
			return true;
		case EBlueprintStatus::BS_Error:
			return ReportError(*Blueprint->GetName(), Rule_3_1_Errors);
		case EBlueprintStatus::BS_UpToDateWithWarnings:
			return ReportError(*Blueprint->GetName(), Rule_3_1_Warnings);
		default:
			return true;
	}
}

bool FBlueprintLinter::HasSaneVariables()
{
	const ULinterSettings* DefaultSettings = GetDefault<ULinterSettings>();
	if (!DefaultSettings->bBPVariableChecks)
	{
		return true;
	}

	check(Blueprint);
	check(!Blueprint->bHasBeenRegenerated || Blueprint->bIsRegeneratingOnLoad || (Blueprint->SkeletonGeneratedClass != NULL));

	bool bHadErrors = false;

	for (FBPVariableDescription Desc : Blueprint->NewVariables)
	{
		FString PropName = Desc.VarName.ToString();
		FText TypeName = UEdGraphSchema_K2::TypeToText(Desc.VarType);
		bool bIsBool = Desc.VarType.PinCategory == UEdGraphSchema_K2::PC_Boolean;

		if (DefaultSettings->bBPVarPascalCase)
		{
			// PascalCase check
			FRegexMatcher Matcher(bIsBool ? PascalBooleanCase : PascalCase, PropName);
			if (!Matcher.FindNext() || Matcher.GetMatchBeginning() != 0 || Matcher.GetMatchEnding() != PropName.Len())
			{
				ReportError(*Blueprint->GetName(), FText::FormatNamed(Rule_3_2_1_2, TEXT("Type"), TypeName, TEXT("Name"), FText::FromString(PropName)));
				bHadErrors = true;
			}
		}

		// Boolean specific checks
		if (bIsBool)
		{
			if (DefaultSettings->bBPVarBoolPrefix)
			{
				if (!PropName.StartsWith(TEXT("b"), ESearchCase::CaseSensitive))
				{
					ReportError(*Blueprint->GetName(), FText::FormatNamed(Rule_3_2_1_3, TEXT("Type"), TypeName, TEXT("Name"), FText::FromString(PropName)));
					bHadErrors = true;
				}
			}

			if (DefaultSettings->bBPVarBoolIsQuestion)
			{
				if (PropName.StartsWith(TEXT("bIs"), ESearchCase::CaseSensitive))
				{
					ReportError(*Blueprint->GetName(), FText::FormatNamed(Rule_3_2_1_4_1, TEXT("Type"), TypeName, TEXT("Name"), FText::FromString(PropName)));
					bHadErrors = true;
				}
			}
		}

		if (DefaultSettings->bBPVarAtomicInclusion)
		{
			// Atomic name self reference check
			if (IsVariableAtomic(Desc) && PropName.Contains(TypeName.ToString()))
			{
				ReportError(*Blueprint->GetName(), FText::FormatNamed(Rule_3_2_1_6, TEXT("Type"), TypeName, TEXT("Name"), FText::FromString(PropName)));
				bHadErrors = true;
			}
		}
		
		// Array checks
		if (Desc.VarType.bIsArray)
		{
			if (DefaultSettings->bBPVarArrayPlurals) // Partial support
			{
				if (PropName.Contains(TEXT("Array"), ESearchCase::CaseSensitive))
				{
					ReportError(*Blueprint->GetName(), FText::FormatNamed(Rule_3_2_1_8, TEXT("Type"), TypeName, TEXT("Name"), FText::FromString(PropName)));
					bHadErrors = true;
				}
			}
		}

		if (DefaultSettings->bBPVarExposeOnSpawnEditable)
		{
			// Expose on Spawn but not Editable check
			if ((Desc.PropertyFlags & CPF_ExposeOnSpawn) == CPF_ExposeOnSpawn && (Desc.PropertyFlags & CPF_DisableEditOnInstance) == CPF_DisableEditOnInstance)
			{
				ReportError(*Blueprint->GetName(), FText::FormatNamed(Rule_3_2_2, TEXT("Type"), TypeName, TEXT("Name"), FText::FromString(PropName)));
				bHadErrors = true;
			}
		}
		
		if (DefaultSettings->bBPVarSaveGameInSaveGame)
		{
			// SaveGame var check
			if ((Desc.PropertyFlags & CPF_SaveGame) == CPF_SaveGame && !Blueprint->GeneratedClass->IsChildOf(USaveGame::StaticClass()))
			{
				ReportError(*Blueprint->GetName(), FText::FormatNamed(Rule_3_2_7, TEXT("Type"), TypeName, TEXT("Name"), FText::FromString(PropName)));
				bHadErrors = true;
			}
		}
		
		if (DefaultSettings->bBPVarSaveNoConfig)
		{
			// Config var check
			if ((Desc.PropertyFlags & CPF_Config) == CPF_Config)
			{
				ReportError(*Blueprint->GetName(), FText::FormatNamed(Rule_3_2_8, TEXT("Type"), TypeName, TEXT("Name"), FText::FromString(PropName)));
				bHadErrors = true;
			}
		}
	}	

	return !bHadErrors;
}

bool FBlueprintLinter::HasEditableVariablesCategorized()
{
	if (!GetDefault<ULinterSettings>()->bBPVariableChecks || !GetDefault<ULinterSettings>()->bBPVarEditableCategories)
	{
		return true;
	}

	check(Blueprint);

	bool bHadErrors = false;

	if (VariableCount >= 5 && EditableVariables.Num() > 0)
	{
		for (FBPVariableDescription Desc : EditableVariables)
		{
			FString PropName = Desc.VarName.ToString();
			FText TypeName = UEdGraphSchema_K2::TypeToText(Desc.VarType);

			if (Desc.Category.IsEmptyOrWhitespace())
			{
				ReportError(*Blueprint->GetName(), FText::FormatNamed(Rule_3_2_3_NoCategory, TEXT("Type"), TypeName, TEXT("Name"), FText::FromString(PropName)));
				bHadErrors = true;
			}

			if (!Desc.Category.ToString().StartsWith(TEXT("Config")))
			{
				ReportError(*Blueprint->GetName(), FText::FormatNamed(Rule_3_2_3_BadCategory, TEXT("Type"), TypeName, TEXT("Name"), FText::FromString(PropName), TEXT("Category"), Desc.Category));
				bHadErrors = true;
			}
		}
	}

	return !bHadErrors;
}

bool FBlueprintLinter::IsVariableAtomic(FBPVariableDescription& VarDesc)
{
	return (VarDesc.VarType.PinCategory == UEdGraphSchema_K2::PC_Boolean
		|| VarDesc.VarType.PinCategory == UEdGraphSchema_K2::PC_Byte
		|| VarDesc.VarType.PinCategory == UEdGraphSchema_K2::PC_Int
		|| VarDesc.VarType.PinCategory == UEdGraphSchema_K2::PC_Float
		|| VarDesc.VarType.PinCategory == UEdGraphSchema_K2::PC_String
		|| VarDesc.VarType.PinCategory == UEdGraphSchema_K2::PC_Enum
		);
}
