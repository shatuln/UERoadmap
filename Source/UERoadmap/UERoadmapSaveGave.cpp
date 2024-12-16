// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmapSaveGave.h"

#include "EngineUtils.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

UUERoadmapSaveGave::UUERoadmapSaveGave()
{
	SaveSlotName = TEXT("DefaultSaveSlot");
    UserIndex = 0;
    
    PlayerLocation = FVector::ZeroVector;
	PlayerRotation = FRotator::ZeroRotator;
}

/*void UUERoadmapSaveGave::SaveGame()
{
	SavedActors.Empty();

	for (FActorIterator It(GetWorld()); It; ++It)
	{
		AActor* Actor = *It;
		// Only interested in our 'gameplay actors', skip actors that are being destroyed
		// Note: You might instead use a dedicated SavableObject interface for Actors you want to save instead of re-using GameplayInterface
		if (!Actor->GetRootComponent()->IsSimulatingPhysics())
		{
			continue;
		}

		FActorSaveData ActorData;
		ActorData.ActorName = Actor->GetFName();
		ActorData.Transform = Actor->GetActorTransform();

		UE_LOG(LogTemp, Display, TEXT("SavedActorName: %s"), *Actor->GetFName().ToString());
		
		// Pass the array to fill with data from Actor
		FMemoryWriter MemWriter(ActorData.ByteData);

		FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
		// Find only variables with UPROPERTY(SaveGame)
		Ar.ArIsSaveGame = true;
		// Converts Actor's SaveGame UPROPERTIES into binary array
		Actor->Serialize(Ar);

		SavedActors.Add(ActorData);
	}

}*/
