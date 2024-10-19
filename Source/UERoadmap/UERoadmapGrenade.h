// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UERoadmapGrenade.generated.h"

UCLASS()
class UEROADMAP_API AUERoadmapGrenade : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUERoadmapGrenade();
	
	UPROPERTY(EditAnywhere, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshGrenade;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
public:
	void OnReleased();

};
