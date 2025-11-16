// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_AdvancedCustomNode.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_CallFunction.h"
#include "UObject/UnrealType.h"
#include "KismetCompiler.h"

#define LOCTEXT_NAMESPACE "K2Node_AdvancedCustomNode"

FText UK2Node_AdvancedCustomNode::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("K2Node_AdvancedCustomNode_NodeTitle", "Super Custom Advanced Node");
}

void UK2Node_AdvancedCustomNode::GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const
{
	UClass* Action = GetClass();
	if (ActionRegistrar.IsOpenForRegistration(Action))
	{
		UBlueprintNodeSpawner* Spawner = UBlueprintNodeSpawner::Create(GetClass());
		ActionRegistrar.AddBlueprintAction(Action, Spawner);
	}
}

void UK2Node_AdvancedCustomNode::AllocateDefaultPins()
{
	Super::AllocateDefaultPins();
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Exec, TEXT("In"));
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Exec, TEXT("Out"));

	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Object, UObject::StaticClass(), TEXT("Object"));
	CreatePin(EGPD_Input, UEdGraphSchema_K2::PC_Name, TEXT("Param"));
	CreatePin(EGPD_Output, UEdGraphSchema_K2::PC_Wildcard, TEXT("Value"));
	
	AnalyzeAndSetOutputPinType();
}

void UK2Node_AdvancedCustomNode::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);

	AnalyzePins(Pin);
	AnalyzeAndSetOutputPinType();
	
}

void UK2Node_AdvancedCustomNode::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	AnalyzePins(Pin);
	AnalyzeAndSetOutputPinType();
}

void UK2Node_AdvancedCustomNode::PostReconstructNode()
{
	Super::PostReconstructNode();
	
	AnalyzePins(FindPinChecked(TEXT("Object")));
	AnalyzePins(FindPinChecked(TEXT("Param")));
	AnalyzeAndSetOutputPinType();
}

void UK2Node_AdvancedCustomNode::GetProperty(UObject* Target, FName PropertyName, int32& OutValue)
{
	unimplemented();
}

void UK2Node_AdvancedCustomNode::AnalyzePins(UEdGraphPin* Pin)
{
	if (Pin && Pin->PinName == TEXT("Param"))
	{
		PropertyName = *Pin->GetDefaultAsString();
	}
	else if (Pin && Pin->PinName == TEXT("Object"))
	{	
		if (!Pin->LinkedTo.IsEmpty())
		{
			UEdGraphPin* LinkedPin = Pin->LinkedTo[0];
			if (LinkedPin->PinType.PinCategory == UEdGraphSchema_K2::PC_Object)
			{
				if (LinkedPin->PinType.PinSubCategory == UEdGraphSchema_K2::PSC_Self)
				{
					if (UBlueprint* Blueprint = GetBlueprint())
					{
						TargetClass = Blueprint->GeneratedClass;
					}
				}
				else
				{
					TargetClass = Cast<UClass>(LinkedPin->PinType.PinSubCategoryObject.Get());
				}
			}
		}
		else if (Pin->DefaultObject != nullptr)
		{
			if (UObject* Object = Pin->DefaultObject)
			{
				TargetClass = Object->GetClass();
			}
		}
		else
		{
			TargetClass = nullptr;
		}
	}
}

void UK2Node_AdvancedCustomNode::AnalyzeAndSetOutputPinType()
{
	UEdGraphPin* OutputPin = FindPin(TEXT("Value"), EGPD_Output);
	
	if (TargetClass && PropertyName.IsValid() && OutputPin)
	{
		TargetProperty = TargetClass->FindPropertyByName(PropertyName);
		if (TargetProperty)
		{
			if (TargetProperty->IsA(FIntProperty::StaticClass()))
			{
				OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Int;
			}
			else if (TargetProperty->IsA(FFloatProperty::StaticClass()))
			{
				OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Real;
			}
			else if (TargetProperty->IsA(FStrProperty::StaticClass()))
			{
				OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_String;
			}
			else if (TargetProperty->IsA(FBoolProperty::StaticClass()))
			{
				OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Boolean;
			}
			else if (FObjectProperty* ObjProp = CastField<FObjectProperty>(TargetProperty))
			{
				OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Object;
				OutputPin->PinType.PinSubCategoryObject = ObjProp->PropertyClass;
			}
		}
	}
	else
	{
		OutputPin->PinType.PinCategory = UEdGraphSchema_K2::PC_Wildcard;
	}
	
	GetGraph()->NotifyGraphChanged();
}

void UK2Node_AdvancedCustomNode::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UK2Node_CallFunction* Call = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	Call->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UK2Node_AdvancedCustomNode, GetProperty), UK2Node_AdvancedCustomNode::StaticClass());
	Call->bIsPureFunc = true;
	Call->AllocateDefaultPins();

	UEdGraphPin* ValuePin = FindPinChecked(TEXT("Value"));
	UEdGraphPin* FuncValuePin = Call->FindPinChecked(TEXT("OutValue"));

	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(TEXT("Object")), *Call->FindPinChecked(TEXT("Target")));
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(TEXT("Param")), *Call->FindPinChecked(TEXT("PropertyName")));
	if (ValuePin->PinType.PinCategory != UEdGraphSchema_K2::PC_Wildcard)
	{
		FuncValuePin->PinType = ValuePin->PinType;
		CompilerContext.MovePinLinksToIntermediate(*ValuePin, *FuncValuePin);
	}
	else
	{
		FuncValuePin->PinType.ResetToDefaults();
		FuncValuePin->PinType.PinCategory = UEdGraphSchema_K2::PC_Byte;
	}
	

	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(TEXT("In")), *Call->GetExecPin());
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(TEXT("Out")), *Call->GetThenPin());

	BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE
