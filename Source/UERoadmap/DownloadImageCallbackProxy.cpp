// Fill out your copyright notice in the Description page of Project Settings.


#include "DownloadImageCallbackProxy.h"

#include "HttpModule.h"
#include "Interfaces/IHttpResponse.h"
#include "Engine/Texture2D.h"
#include "IImageWrapper.h"
#include "IImageWrapperModule.h"
#include "Modules/ModuleManager.h"

UDownloadImageCallbackProxy* UDownloadImageCallbackProxy::DownloadImage(const FString& URL)
{
	UDownloadImageCallbackProxy* Proxy = NewObject<UDownloadImageCallbackProxy>();
	Proxy->ImageURL = URL;
	return Proxy;
}

void UDownloadImageCallbackProxy::Activate()
{
	Super::Activate();

	FHttpModule* Http = &FHttpModule::Get();
	if (!Http)
	{
		return;
	}

	TSharedRef<IHttpRequest> Request = Http->CreateRequest();
	Request->OnProcessRequestComplete().BindUObject(this, &UDownloadImageCallbackProxy::OnResponseReceived);
	Request->SetURL(ImageURL);
	Request->SetVerb("GET");
	Request->ProcessRequest();
}

void UDownloadImageCallbackProxy::OnResponseReceived(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bWasSuccessful)
{
	if (!bWasSuccessful || !Response.IsValid())
	{
		OnFail.Broadcast();
		return;
	}

	const TArray<uint8>& ImageData = Response->GetContent();
	IImageWrapperModule& ImageWrapperModule = FModuleManager::LoadModuleChecked<IImageWrapperModule>(FName("ImageWrapper"));
	TSharedPtr<IImageWrapper> ImageWrapper = ImageWrapperModule.CreateImageWrapper(EImageFormat::PNG);

	if (ImageWrapper.IsValid() && ImageWrapper->SetCompressed(ImageData.GetData(), ImageData.Num()))
	{
		TArray64<uint8> UncompressedBGRA;
		if (ImageWrapper->GetRaw(UncompressedBGRA))
		{
			UTexture2D* Texture = UTexture2D::CreateTransient(ImageWrapper->GetWidth(), ImageWrapper->GetHeight(), PF_B8G8R8A8);
			if (Texture)
			{
				void* TextureData = Texture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE);
				FMemory::Memcpy(TextureData, UncompressedBGRA.GetData(), UncompressedBGRA.Num());
				Texture->GetPlatformData()->Mips[0].BulkData.Unlock();
				Texture->UpdateResource();

				OnSuccess.Broadcast(Texture);
				return;
			}
		}
	}

	OnFail.Broadcast();
}
