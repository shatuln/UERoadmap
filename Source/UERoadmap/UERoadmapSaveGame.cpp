// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmapSaveGame.h"

UUERoadmapSaveGame::UUERoadmapSaveGame()
{
	SaveSlotName = TEXT("DefaultSaveSlot");
    UserIndex = 0;
    
    PlayerLocation = FVector::ZeroVector;
	PlayerRotation = FRotator::ZeroRotator;
}
