// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmapStreamingManager.h"

#include "EngineUtils.h"
#include "UERoadmapAssetStreamingComponent.h"

UUERoadmapStreamingManager* UUERoadmapStreamingManager::Instance = nullptr;

template <typename TComponent>
void GetAllActorsWithComponent(UWorld* World, TArray<AActor*>& OutActors)
{
	OutActors.Empty();

	for (FActorIterator It(World); It; ++It)
	{
		AActor* Actor = *It;
		if (Actor && Actor->FindComponentByClass<TComponent>())
		{
			OutActors.Add(Actor);
		}
	}
}

UUERoadmapStreamingManager* UUERoadmapStreamingManager::Get(UObject* WorldContextObject)
{
	if (!Instance)
	{
		Instance = NewObject<UUERoadmapStreamingManager>();
		Instance->AddToRoot();
	}
	return Instance;
}

void UUERoadmapStreamingManager::UpdateStreaming(AActor* PlayerActor)
{
	if (!PlayerActor) return;

	const float LoadDistance = 1000.0f;
	const float UnloadDistance = 1500.0f;

	TArray<AActor*> FoundActors;
	GetAllActorsWithComponent<UUERoadmapAssetStreamingComponent>(PlayerActor->GetWorld(), FoundActors);

	for (AActor* Actor : FoundActors)
	{
		if (!Actor) continue;

		float Distance = FVector::Dist(PlayerActor->GetActorLocation(), Actor->GetActorLocation());
		bool bShouldLoad = Distance <= LoadDistance;
		bool bShouldUnload = Distance > UnloadDistance;

		if (bShouldLoad)
		{
			OnStreamingRequest.Broadcast(Actor, true);
		}
		else if (bShouldUnload)
		{
			OnStreamingRequest.Broadcast(Actor, false);
		}
	}
}
