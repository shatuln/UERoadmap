// Copyright Epic Games, Inc. All Rights Reserved.


#include "UERoadmapPlayerController.h"

#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UERoadmapHUD.h"
#include "Engine/LocalPlayer.h"

void AUERoadmapPlayerController::ToggleMenu()
{
	AUERoadmapHUD* HUD = GetHUD<AUERoadmapHUD>();

	if (HUD != nullptr && IsLocalController())
	{
		if (!bIsMenuToggled)
		{
			bIsMenuToggled = true;
			SetPause(true);
			SetInputMode(FInputModeGameAndUI());
			SetShowMouseCursor(true);

			SetIgnoreLookInput(true);
			SetIgnoreMoveInput(true);
			HUD->ShowMainMenu();
		}
		else
		{
			bIsMenuToggled = false;
		
			SetInputMode(FInputModeGameOnly());
			SetShowMouseCursor(false);

			SetIgnoreLookInput(false);
			SetIgnoreMoveInput(false);
			HUD->HideMainMenu();
			SetPause(false);
		}
	}
}

void AUERoadmapPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(InputComponent))
	{
		EnhancedInput->BindAction(ToggleMenuAction, ETriggerEvent::Completed, this, &AUERoadmapPlayerController::ToggleMenu);
		ToggleMenuAction->bTriggerWhenPaused = true;
	}
}

void AUERoadmapPlayerController::BeginPlay()
{
	Super::BeginPlay();

	bIsMenuToggled = false;
	
	// get the enhanced input subsystem
	if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
	{
		// add the mapping context so we get controls
		Subsystem->AddMappingContext(InputMappingContext, 0);
	}
}
