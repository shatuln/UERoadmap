// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Interfaces/IHttpRequest.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "DownloadImageCallbackProxy.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDownloadImageSuccess, UTexture2D*, Texture);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDownloadImageFailure);

/**
 * 
 */
UCLASS()
class UEROADMAP_API UDownloadImageCallbackProxy : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()

	UPROPERTY(BlueprintAssignable)
	FDownloadImageSuccess OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FDownloadImageFailure OnFail;

	UFUNCTION(BlueprintCallable, meta = (BlueprintInternalUseOnly = "true"))
	static UDownloadImageCallbackProxy* DownloadImage(const FString& URL);

	virtual void Activate() override;

private:
	FString ImageURL;
	void OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful);
};
