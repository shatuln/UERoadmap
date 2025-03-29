// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "UERoadmapStreamingManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FStreamingRequestSignature, AActor*, RequestingActor, bool, bLoad);

/**
 * 
 */
UCLASS()
class UEROADMAP_API UUERoadmapStreamingManager : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable, Category="Streaming")
	FStreamingRequestSignature OnStreamingRequest;
	
	static UUERoadmapStreamingManager* Get(UObject* WorldContextObject);

	void UpdateStreaming(AActor* PlayerActor);

private:
	static UUERoadmapStreamingManager* Instance;
	
};
