// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "K2Node.h"
#include "K2Node_CustomDelay.generated.h"

/**
 * 
 */
UCLASS()
class UEROADMAP_API UK2Node_CustomDelay : public UK2Node
{
	GENERATED_BODY()
	
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void GetMenuActions(FBlueprintActionDatabaseRegistrar& ActionRegistrar) const override;
	virtual void AllocateDefaultPins() override;
	virtual void ExpandNode(FKismetCompilerContext& CompilerContext, UEdGraph* SourceGraph) override;

	UFUNCTION(BlueprintCallable, meta=(WorldContext="WorldContextObject", Latent, LatentInfo="LatentInfo"))
	static void WaitForKey(const UObject* WorldContextObject, FLatentActionInfo LatentInfo, FKey Key);
};

class FWaitForKeyLatentAction : public FPendingLatentAction
{
public:
	FName ExecutionFunction;
	int32 OutputLink;
	FWeakObjectPtr CallbackTarget;

	FKey KeyToWaitFor;
	bool bTriggered = false;

	FWaitForKeyLatentAction(const FKey& InKey, const FLatentActionInfo& LatentInfo)
		: ExecutionFunction(LatentInfo.ExecutionFunction)
		, OutputLink(LatentInfo.Linkage)
		, CallbackTarget(LatentInfo.CallbackTarget)
		, KeyToWaitFor(InKey)
	{}

	virtual void UpdateOperation(FLatentResponse& Response) override
	{
		bool bPressed = false;

		if (UWorld* World = GEngine->GetWorldFromContextObjectChecked(CallbackTarget.Get()))
		{
			if (APlayerController* PC = World->GetFirstPlayerController())
			{
				bPressed = PC->WasInputKeyJustPressed(KeyToWaitFor);
			}
		}

		Response.FinishAndTriggerIf(bPressed, ExecutionFunction, OutputLink, CallbackTarget);
	}
};
