// Copyright Epic Games, Inc. All Rights Reserved.

#include "DynamicStairsPluginEditorMode.h"
#include "DynamicStairsPluginEditorModeToolkit.h"
#include "EdModeInteractiveToolsContext.h"
#include "InteractiveToolManager.h"
#include "DynamicStairsPluginEditorModeCommands.h"


//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
// AddYourTool Step 1 - include the header file for your Tools here
//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 
#include "Tools/DynamicStairsPluginSimpleTool.h"
#include "Tools/DynamicStairsPluginInteractiveTool.h"
#include "Tools/DynamicStairsPluginMyStairsTool.h"

// step 2: register a ToolBuilder in FDynamicStairsPluginEditorMode::Enter() below


#define LOCTEXT_NAMESPACE "DynamicStairsPluginEditorMode"

const FEditorModeID UDynamicStairsPluginEditorMode::EM_DynamicStairsPluginEditorModeId = TEXT("EM_DynamicStairsPluginEditorMode");

FString UDynamicStairsPluginEditorMode::SimpleToolName = TEXT("DynamicStairsPlugin_ActorInfoTool");
FString UDynamicStairsPluginEditorMode::InteractiveToolName = TEXT("DynamicStairsPlugin_MeasureDistanceTool");
FString UDynamicStairsPluginEditorMode::MyStairsToolName = TEXT("DynamicStairsPlugin_MyCoolStairsTool");


UDynamicStairsPluginEditorMode::UDynamicStairsPluginEditorMode()
{
	FModuleManager::Get().LoadModule("EditorStyle");

	// appearance and icon in the editing mode ribbon can be customized here
	Info = FEditorModeInfo(UDynamicStairsPluginEditorMode::EM_DynamicStairsPluginEditorModeId,
		LOCTEXT("ModeName", "DynamicStairsPlugin"),
		FSlateIcon(),
		true);
}


UDynamicStairsPluginEditorMode::~UDynamicStairsPluginEditorMode()
{
}


void UDynamicStairsPluginEditorMode::ActorSelectionChangeNotify()
{
}

void UDynamicStairsPluginEditorMode::Enter()
{
	UEdMode::Enter();

	//////////////////////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////
	// AddYourTool Step 2 - register the ToolBuilders for your Tools here.
	// The string name you pass to the ToolManager is used to select/activate your ToolBuilder later.
	//////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////// 
	const FDynamicStairsPluginEditorModeCommands& SampleToolCommands = FDynamicStairsPluginEditorModeCommands::Get();

	RegisterTool(SampleToolCommands.SimpleTool, SimpleToolName, NewObject<UDynamicStairsPluginSimpleToolBuilder>(this));
	RegisterTool(SampleToolCommands.InteractiveTool, InteractiveToolName, NewObject<UDynamicStairsPluginInteractiveToolBuilder>(this));
	RegisterTool(SampleToolCommands.MyStairsTool, MyStairsToolName, NewObject<UDynamicStairsPluginMyStairsToolBuilder>(this));

	// active tool type is not relevant here, we just set to default
	GetToolManager()->SelectActiveToolType(EToolSide::Left, SimpleToolName);
}

void UDynamicStairsPluginEditorMode::CreateToolkit()
{
	Toolkit = MakeShareable(new FDynamicStairsPluginEditorModeToolkit);
}

TMap<FName, TArray<TSharedPtr<FUICommandInfo>>> UDynamicStairsPluginEditorMode::GetModeCommands() const
{
	return FDynamicStairsPluginEditorModeCommands::Get().GetCommands();
}

#undef LOCTEXT_NAMESPACE
