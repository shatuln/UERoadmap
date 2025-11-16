// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_AdvancedCustomNode.generated.h"

/**
 * 
 */
UCLASS()
class UEROADMAP_API UK2Node_AdvancedCustomNode : public UK2Node
{
	GENERATED_BODY()

	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void AllocateDefaultPins() override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void NotifyPinConnectionListChanged(UEdGraphPin* Pin) override;
	virtual void PostReconstructNode() override;

	UFUNCTION(BlueprintCallable, CustomThunk, meta=(SetParam="OutValue"), Category="Reflection")
	static void GetProperty(UObject* Target, FName PropertyName, int32& OutValue);

	DECLARE_FUNCTION(execGetProperty)
	{
		P_GET_OBJECT(UObject, Target);
		P_GET_PROPERTY(FNameProperty, PropertyName);

		Stack.MostRecentProperty = nullptr;
		Stack.MostRecentPropertyAddress = nullptr;
	
		Stack.StepCompiledIn<FProperty>(nullptr);
		void* OutValueAddr = Stack.MostRecentPropertyAddress;

		P_FINISH;
		P_NATIVE_BEGIN;

		if (Target && OutValueAddr)
		{
			if (FProperty* Prop = Target->GetClass()->FindPropertyByName(PropertyName))
			{
				void* SrcPtr = Prop->ContainerPtrToValuePtr<void>(Target);
				if (SrcPtr)
				{
					Prop->CopyCompleteValueFromScriptVM(OutValueAddr, SrcPtr);
				}
			}
		}

		P_NATIVE_END;
	}
	
private:
	UPROPERTY()
	UClass* TargetClass = nullptr;

	UPROPERTY()
	FName PropertyName;

	FProperty* TargetProperty = nullptr;

	void AnalyzePins(UEdGraphPin* Pin);
	void AnalyzeAndSetOutputPinType();
	
};
