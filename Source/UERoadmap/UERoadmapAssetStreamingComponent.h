// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UERoadmapAssetStreamingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UEROADMAP_API UUERoadmapAssetStreamingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUERoadmapAssetStreamingComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	UPROPERTY(EditAnywhere, Category="Streaming")
	TSoftObjectPtr<UStaticMesh> MeshAsset;

	UPROPERTY()
	UStaticMesh* LoadedMesh;
	
	bool bIsLoaded = false;
	
	void LoadMeshAsync();
	void UnloadMesh();
	void OnMeshLoaded();
	
	UFUNCTION()
	void OnStreamingRequest(AActor* RequestingActor, bool bLoad);
	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
