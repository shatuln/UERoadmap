// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "UERoadmapSaveGave.generated.h"

USTRUCT()
struct FActorSaveData
{
	GENERATED_BODY()

public:
	/* Identifier for which Actor this belongs to */
	UPROPERTY()
	FName ActorName;

	/* For movable Actors, keep location,rotation,scale. */
	UPROPERTY()
	FTransform Transform;

	/* Contains all 'SaveGame' marked variables of the Actor */
	UPROPERTY()
	TArray<uint8> ByteData;
};

/**
 * 
 */
UCLASS()
class UEROADMAP_API UUERoadmapSaveGave : public USaveGame
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

	UPROPERTY()
	TArray<FActorSaveData> SavedActors;
	
	UUERoadmapSaveGave();
	//void SaveGame();
};
