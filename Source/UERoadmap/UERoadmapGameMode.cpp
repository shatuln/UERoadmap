// Copyright Epic Games, Inc. All Rights Reserved.

#include "UERoadmapGameMode.h"
#include "UERoadmapCharacter.h"
#include "UERoadmapHUD.h"
#include "Kismet/GameplayStatics.h"
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
