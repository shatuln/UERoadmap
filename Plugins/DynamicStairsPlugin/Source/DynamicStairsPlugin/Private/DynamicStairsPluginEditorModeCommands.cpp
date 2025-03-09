// Copyright Epic Games, Inc. All Rights Reserved.

#include "DynamicStairsPluginEditorModeCommands.h"
#include "DynamicStairsPluginEditorMode.h"
#include "EditorStyleSet.h"

#define LOCTEXT_NAMESPACE "DynamicStairsPluginEditorModeCommands"

FDynamicStairsPluginEditorModeCommands::FDynamicStairsPluginEditorModeCommands()
	: TCommands<FDynamicStairsPluginEditorModeCommands>("DynamicStairsPluginEditorMode",
		NSLOCTEXT("DynamicStairsPluginEditorMode", "DynamicStairsPluginEditorModeCommands", "DynamicStairsPlugin Editor Mode"),
		NAME_None,
		FAppStyle::GetAppStyleSetName())
{
}

void FDynamicStairsPluginEditorModeCommands::RegisterCommands()
{
	TArray <TSharedPtr<FUICommandInfo>>& ToolCommands = Commands.FindOrAdd(NAME_Default);

	UI_COMMAND(SimpleTool, "Show Actor Info", "Opens message box with info about a clicked actor", EUserInterfaceActionType::Button, FInputChord());
	ToolCommands.Add(SimpleTool);

	UI_COMMAND(InteractiveTool, "Measure Distance", "Measures distance between 2 points (click to set origin, shift-click to set end point)", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(InteractiveTool);

	UI_COMMAND(MyStairsTool, "My super stairs tool", "WOW!!!", EUserInterfaceActionType::ToggleButton, FInputChord());
	ToolCommands.Add(MyStairsTool);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> FDynamicStairsPluginEditorModeCommands::GetCommands()
{
	return FDynamicStairsPluginEditorModeCommands::Get().Commands;
}

#undef LOCTEXT_NAMESPACE
