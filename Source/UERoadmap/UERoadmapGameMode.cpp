// Copyright Epic Games, Inc. All Rights Reserved.

#include "UERoadmapGameMode.h"

#include "EngineUtils.h"
#include "UERoadmapCharacter.h"
#include "UERoadmapHUD.h"
#include "UERoadmapSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "UObject/ConstructorHelpers.h"

AUERoadmapGameMode::AUERoadmapGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}

void AUERoadmapGameMode::BeginPlay()
{
	Super::BeginPlay();

	//TODO: Commented till buttons work
	
	//APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	//AUERoadmapHUD* HUD = PC->GetHUD<AUERoadmapHUD>();
	//HUD->ShowMainMenu();
}

void AUERoadmapGameMode::SaveGameInGameMode(UUERoadmapSaveGame* SaveGameInstance)
{
	if (SaveGameInstance)
	{
		SaveGameInstance->SavedActors.Empty();

		for (FActorIterator It(GetWorld()); It; ++It)
		{
			AActor* Actor = *It;
			// Only interested in our 'gameplay actors', skip actors that are being destroyed
			// Note: You might instead use a dedicated SavableObject interface for Actors you want to save instead of re-using GameplayInterface
			UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Actor->GetRootComponent());
			if (MeshComp)
			{
				if (!MeshComp->IsSimulatingPhysics())
				{
					continue;
				}
				
				FActorSaveData ActorData;
				ActorData.ActorName = Actor->GetFName();
				ActorData.Transform = Actor->GetActorTransform();

				ActorData.LinearImpulse = MeshComp->GetMass() * MeshComp->GetPhysicsLinearVelocity();
				ActorData.AngularImpulse = MeshComp->GetPhysicsAngularVelocityInDegrees();

				if (Actor->GetClass()->IsChildOf(AUERoadmapGrenade::StaticClass()))
				{
					AUERoadmapGrenade* Grenade = Cast<AUERoadmapGrenade>(Actor);
					if (Grenade)
					{
						Grenade->SaveGameOnGrenade(ActorData);
					}
				}

				UE_LOG(LogTemp, Display, TEXT("SavedActorName: %s"), *Actor->GetFName().ToString());
		
				// Pass the array to fill with data from Actor
				FMemoryWriter MemWriter(ActorData.ByteData);

				FObjectAndNameAsStringProxyArchive Ar(MemWriter, true);
				// Find only variables with UPROPERTY(SaveGame)
				Ar.ArIsSaveGame = true;
				// Converts Actor's SaveGame UPROPERTIES into binary array
				Actor->Serialize(Ar);

				SaveGameInstance->SavedActors.Add(ActorData);
			}
		}
	}
}

void AUERoadmapGameMode::LoadGameInGameMode(UUERoadmapSaveGame* LoadGameInstance)
{
	if (LoadGameInstance)
	{
		for (const FActorSaveData& ActorData : LoadGameInstance->SavedActors)
		{
			for (FActorIterator It(GetWorld()); It; ++It)
			{
				AActor* Actor = *It;
				if (Actor && Actor->GetName() == ActorData.ActorName)
				{
					Actor->SetActorTransform(ActorData.Transform);

					UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Actor->GetRootComponent());

					if (MeshComp)
					{
						MeshComp->SetPhysicsLinearVelocity(ActorData.LinearImpulse / MeshComp->GetMass());
						MeshComp->SetPhysicsAngularVelocityInDegrees(ActorData.AngularImpulse);
					}

					if (Actor->GetClass()->IsChildOf(AUERoadmapGrenade::StaticClass()))
					{
						AUERoadmapGrenade* Grenade = Cast<AUERoadmapGrenade>(Actor);
						if (Grenade)
						{
							Grenade->LoadGameOnGrenade(ActorData);
						}
					}

					FMemoryReader MemReader(ActorData.ByteData);

					FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
					Ar.ArIsSaveGame = true;
					// Convert binary array back into actor's variables
					Actor->Serialize(Ar);

					break;
				}
			}
		}
		UE_LOG(LogTemp, Log, TEXT("Game Loaded: %d actors"), LoadGameInstance->SavedActors.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No SaveGame found!"));
	}
}
