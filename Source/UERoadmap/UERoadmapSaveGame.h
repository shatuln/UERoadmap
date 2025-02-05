// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UERoadmapSaveGame.generated.h"

USTRUCT()
struct FGrenadeSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	float RemainingTime;

	UPROPERTY()
	FTimerHandle ExplodeTimerHandle;
};

USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TSubclassOf<AActor> ActorClass;

	/* For movable Actors, keep location,rotation,scale. */
	UPROPERTY()
	FTransform Transform = FTransform();
	
	UPROPERTY()
	FVector LinearImpulse = FVector::ZeroVector;

	UPROPERTY()
	FVector AngularImpulse = FVector::ZeroVector;

	/* Contains all 'SaveGame' marked variables of the Actor */
	UPROPERTY()
	TArray<uint8> ByteData;

	// Grenade save data
	UPROPERTY()
	FGrenadeSaveData GrenadeSaveData;
};

/**
 * 
 */
UCLASS()
class UEROADMAP_API UUERoadmapSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere)
    FString SaveSlotName;

	UPROPERTY(VisibleAnywhere)
	uint32 UserIndex;

	UPROPERTY(VisibleAnywhere)
	FVector PlayerLocation;
	
	UPROPERTY(VisibleAnywhere)
	FRotator PlayerRotation;

	UPROPERTY(VisibleAnywhere)
	int PlayerAmmoCount;

	UPROPERTY()
	TMap<FString, FActorSaveData> SavedActors;
	
	UUERoadmapSaveGame();
};
