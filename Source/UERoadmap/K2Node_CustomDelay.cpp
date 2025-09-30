// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_CustomDelay.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "K2Node_CustomDelay"

FText UK2Node_CustomDelay::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("K2Node_CustomDelay_NodeTitle", "Super Custom Delay");
}

void UK2Node_CustomDelay::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	Super::GetMenuActions(ActionRegistrar);
	UClass* Action = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(Action))
	{
		UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(GetClass());
		ActionRegistrar.AddBlueprintAction(Action, Spawner);
	}
}

void UK2Node_CustomDelay::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, TEXT("In"));
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, TEXT("Out"));

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Struct, TBaseStructure<FKey>::Get(), TEXT("Key"));
}

void UK2Node_CustomDelay::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);
	
	UK2Node_CallFunction* CallFuncNode = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	CallFuncNode->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UK2Node_CustomDelay, WaitForKey), UK2Node_CustomDelay::StaticClass());
	CallFuncNode->AllocateDefaultPins();

	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(TEXT("In")), *CallFuncNode->GetExecPin());
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(TEXT("Key")), *CallFuncNode->FindPinChecked(TEXT("Key")));
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(TEXT("Out")), *CallFuncNode->GetThenPin());

	BreakAllNodeLinks();
}

void UK2Node_CustomDelay::WaitForKey(const UObject* WorldContextObject, FLatentActionInfo LatentInfo, FKey Key)
{
	if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(WorldContextObject))
	{
		FLatentActionManager& LatentManager = World->GetLatentActionManager();

		if (LatentManager.FindExistingAction<FWaitForKeyLatentAction>(LatentInfo.CallbackTarget, LatentInfo.UUID) == nullptr)
		{
			LatentManager.AddNewAction(LatentInfo.CallbackTarget, LatentInfo.UUID,
				new FWaitForKeyLatentAction(Key, LatentInfo));
		}
	}
}
