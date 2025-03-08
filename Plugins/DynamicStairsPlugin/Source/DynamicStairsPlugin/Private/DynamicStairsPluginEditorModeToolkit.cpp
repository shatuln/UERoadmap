// Copyright Epic Games, Inc. All Rights Reserved.

#include "DynamicStairsPluginEditorModeToolkit.h"
#include "DynamicStairsPluginEditorMode.h"
#include "Engine/Selection.h"

#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"
#include "IDetailsView.h"
#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "DynamicStairsPluginEditorModeToolkit"

FDynamicStairsPluginEditorModeToolkit::FDynamicStairsPluginEditorModeToolkit()
{
}

void FDynamicStairsPluginEditorModeToolkit::Init(const TSharedPtr<IToolkitHost>& InitToolkitHost, TWeakObjectPtr<UEdMode> InOwningMode)
{
	FModeToolkit::Init(InitToolkitHost, InOwningMode);
}

void FDynamicStairsPluginEditorModeToolkit::GetToolPaletteNames(TArray<FName>& PaletteNames) const
{
	PaletteNames.Add(NAME_Default);
}


FName FDynamicStairsPluginEditorModeToolkit::GetToolkitFName() const
{
	return FName("DynamicStairsPluginEditorMode");
}

FText FDynamicStairsPluginEditorModeToolkit::GetBaseToolkitName() const
{
	return LOCTEXT("DisplayName", "DynamicStairsPluginEditorMode Toolkit");
}

#undef LOCTEXT_NAMESPACE
