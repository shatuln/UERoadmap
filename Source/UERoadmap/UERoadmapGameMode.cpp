// Copyright Epic Games, Inc. All Rights Reserved.

#include "UERoadmapGameMode.h"

#include "EngineUtils.h"
#include "UERoadmapCharacter.h"
#include "UERoadmapHUD.h"
#include "UERoadmapSaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"
#include "OnlineSubsystem.h"
#include "Interfaces/OnlineFriendsInterface.h"
#include "Interfaces/OnlineIdentityInterface.h"
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

	GetSteamFriendsList();
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

				FString ActorName = Actor->GetName();
				
				FActorSaveData ActorData;
				ActorData.ActorClass = Actor->GetClass();
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

				SaveGameInstance->SavedActors.Add(ActorName, ActorData);
			}
		}
	}
}

void AUERoadmapGameMode::LoadGameInGameMode(UUERoadmapSaveGame* LoadGameInstance)
{
	if (LoadGameInstance)
	{
		ClearSavedActors();
		for (const TPair<FString, FActorSaveData>& SavedActor : LoadGameInstance->SavedActors)
		{
			FString ActorName = SavedActor.Key;
			FActorSaveData ActorData = SavedActor.Value;
			
			AActor* ExistingActor = FindActorByName(ActorName);
			if (!ExistingActor)
			{
				FActorSpawnParameters SpawnParams;
				
				SpawnParams.Name = FName(*ActorName);
				SpawnParams.NameMode = FActorSpawnParameters::ESpawnActorNameMode::Requested;
				SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

				if (ActorData.ActorClass && ActorData.ActorClass->IsValidLowLevel())
				{
					ExistingActor = GetWorld()->SpawnActor<AActor>(ActorData.ActorClass, ActorData.Transform, SpawnParams);
					ExistingActor->Tags.Add(FName(TEXT("SpawnedActorWithUniqueName")));
				}
				if (ExistingActor)
				{
					UE_LOG(LogTemp, Log, TEXT("Spawned Actor: %s"), *ActorName);
				}
				else
				{
					UE_LOG(LogTemp, Warning, TEXT("Failed to spawn Actor: %s"), *ActorName);
				}
			}
			
			if (ExistingActor)
			{
				ExistingActor->SetActorTransform(ActorData.Transform);

				UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(ExistingActor->GetRootComponent());

				if (ExistingActor->GetClass()->IsChildOf(AUERoadmapGrenade::StaticClass()))
				{
					AUERoadmapGrenade* Grenade = Cast<AUERoadmapGrenade>(ExistingActor);
					if (Grenade)
					{
						Grenade->LoadGameOnGrenade(ActorData);
					}
				}
				
				if (MeshComp && MeshComp->IsSimulatingPhysics())
				{
					MeshComp->SetPhysicsLinearVelocity(ActorData.LinearImpulse / MeshComp->GetMass());
					MeshComp->SetPhysicsAngularVelocityInDegrees(ActorData.AngularImpulse);
				}

				FMemoryReader MemReader(ActorData.ByteData);

				FObjectAndNameAsStringProxyArchive Ar(MemReader, true);
				Ar.ArIsSaveGame = true;
				// Convert binary array back into actor's variables
				ExistingActor->Serialize(Ar);
			}
			
		}
		UE_LOG(LogTemp, Log, TEXT("Game Loaded: %d actors"), LoadGameInstance->SavedActors.Num());
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No SaveGame found!"));
	}
}

AActor* AUERoadmapGameMode::FindActorByName(const FString& ActorName) const
{
	for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
	{
		if (ActorItr->GetName() == ActorName)
		{
			return *ActorItr;
		}
	}
	return nullptr;
}

void AUERoadmapGameMode::ClearSavedActors()
{
	TArray<AActor*> ActorsToDelete;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName(TEXT("SpawnedActorWithUniqueName")), ActorsToDelete);

	for (AActor* Actor : ActorsToDelete)
	{
		if (Actor)
		{
			UE_LOG(LogTemp, Log, TEXT("Destroying actor: %s"), *Actor->GetName());
			Actor->Destroy();
		}
	}
}

void AUERoadmapGameMode::GetSteamFriendsList()
{
	SteamFriendsList.Push(TEXT("FriendsListFirstItem"));
	IOnlineSubsystem* Subsystem = IOnlineSubsystem::Get(STEAM_SUBSYSTEM); // or nullptr for default
	if (!Subsystem) return;

	IOnlineIdentityPtr Identity = Subsystem->GetIdentityInterface();
	IOnlineFriendsPtr Friends = Subsystem->GetFriendsInterface();

	if (!Identity.IsValid() || !Friends.IsValid()) return;

	TSharedPtr<const FUniqueNetId> UserId = Identity->GetUniquePlayerId(0);
	if (!UserId.IsValid()) return;
	
	TArray<TSharedRef<FOnlineFriend>> FriendList;
	Friends->GetFriendsList(0, TEXT("FriendList"), FriendList);

	SteamFriendsList.Push(Identity->GetPlayerNickname(0));
	for (const TSharedRef<FOnlineFriend>& Friend : FriendList)
	{
		FString Name = Friend->GetDisplayName();
		FString Status = TEXT("Online/NotOnline");
		UE_LOG(LogTemp, Log, TEXT("Friend: %s (%s)"), *Name, *Status);

		SteamFriendsList.Push(Name);
	}
}
