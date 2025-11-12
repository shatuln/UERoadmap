// Fill out your copyright notice in the Description page of Project Settings.


#include "K2Node_AdvancedCustomNode.h"
#include "BlueprintActionDatabaseRegistrar.h"
#include "BlueprintNodeSpawner.h"
#include "K2Node_CallFunction.h"
#include "KismetCompiler.h"

class UProperty;

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
}

void UK2Node_AdvancedCustomNode::PinDefaultValueChanged(UEdGraphPin* Pin)
{
	Super::PinDefaultValueChanged(Pin);
	
	if (Pin && Pin->PinName == TEXT("Param"))
	{
		PropertyName = *Pin->GetDefaultAsString();
	}

	AnalizePins(Pin);
	AnalyzeAndSetOutputPinType();
	
}

void UK2Node_AdvancedCustomNode::NotifyPinConnectionListChanged(UEdGraphPin* Pin)
{
	Super::NotifyPinConnectionListChanged(Pin);

	AnalizePins(Pin);
	AnalyzeAndSetOutputPinType();
}

void UK2Node_AdvancedCustomNode::GetProperty(UObject* Target, FName PropertyName, int32& OutValue)
{
	unimplemented();
}

void UK2Node_AdvancedCustomNode::AnalizePins(UEdGraphPin* Pin)
{
	if (Pin && Pin->PinName == TEXT("Object"))
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
			//bool Value = BoolProperty->GetPropertyValue_InContainer(TargetClass);
			
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
	
	GetGraph()->NotifyGraphChanged();
}

void* UK2Node_AdvancedCustomNode::SetOutputBoolValue()
{
	void* Out = nullptr;
	UObject* Cont = nullptr;
	void* SrcPtr = TargetProperty->ContainerPtrToValuePtr<void>(Cont);
	if (SrcPtr)
	{
		TargetProperty->CopyCompleteValue(Out, SrcPtr);
	}

	return Out;
}

void UK2Node_AdvancedCustomNode::ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph)
{
	Super::ExpandNode(CompilerContext, SourceGraph);

	UK2Node_CallFunction* Call = CompilerContext.SpawnIntermediateNode<UK2Node_CallFunction>(this, SourceGraph);
	Call->FunctionReference.SetExternalMember(GET_FUNCTION_NAME_CHECKED(UK2Node_AdvancedCustomNode, GetProperty), UK2Node_AdvancedCustomNode::StaticClass());
	
	Call->AllocateDefaultPins();

	CompilerContext.MovePinLinksToIntermediate(*FindPin(TEXT("Object")), *Call->FindPin(TEXT("Target")));
	CompilerContext.MovePinLinksToIntermediate(*FindPin(TEXT("Param")), *Call->FindPin(TEXT("PropertyName")));
	CompilerContext.MovePinLinksToIntermediate(*FindPin(TEXT("Value")), *Call->FindPin(TEXT("OutValue")));

	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(TEXT("In")), *Call->GetExecPin());
	CompilerContext.MovePinLinksToIntermediate(*FindPinChecked(TEXT("Out")), *Call->GetThenPin());

	BreakAllNodeLinks();
}

#undef LOCTEXT_NAMESPACE
