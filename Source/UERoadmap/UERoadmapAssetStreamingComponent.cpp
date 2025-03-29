// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmapAssetStreamingComponent.h"

#include "UERoadmapStreamingManager.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

// Sets default values for this component's properties
UUERoadmapAssetStreamingComponent::UUERoadmapAssetStreamingComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


// Called when the game starts
void UUERoadmapAssetStreamingComponent::BeginPlay()
{
	Super::BeginPlay();

	if (UUERoadmapStreamingManager* StreamingManager = UUERoadmapStreamingManager::Get(this))
	{
		StreamingManager->OnStreamingRequest.AddDynamic(this, &UUERoadmapAssetStreamingComponent::OnStreamingRequest);
	}
	
}

void UUERoadmapAssetStreamingComponent::LoadMeshAsync()
{
	if (bIsLoaded || MeshAsset.IsNull()) return;

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(
		MeshAsset.ToSoftObjectPath(),
		FStreamableDelegate::CreateUObject(this, &UUERoadmapAssetStreamingComponent::OnMeshLoaded)
	);
}

void UUERoadmapAssetStreamingComponent::UnloadMesh()
{
	if (!bIsLoaded) return;
	
	if (UStaticMeshComponent* MeshComp = GetOwner()->FindComponentByClass<UStaticMeshComponent>())
    {
        MeshComp->SetStaticMesh(nullptr);
    }

	LoadedMesh = nullptr;
    bIsLoaded = false;
}

void UUERoadmapAssetStreamingComponent::OnMeshLoaded()
{
	LoadedMesh = MeshAsset.Get();
	if (LoadedMesh)
	{
		if (UStaticMeshComponent* MeshComp = GetOwner()->FindComponentByClass<UStaticMeshComponent>())
		{
			MeshComp->SetStaticMesh(LoadedMesh);
		}

		bIsLoaded = true;
	}
}

void UUERoadmapAssetStreamingComponent::OnStreamingRequest(AActor* RequestingActor, bool bLoad)
{
	if (GetOwner() == RequestingActor)
	{
		if (bLoad)
		{
			LoadMeshAsync();
		}
		else
		{
			UnloadMesh();
		}
	}
}


// Called every frame
void UUERoadmapAssetStreamingComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

