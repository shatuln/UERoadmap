// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmapHUD.h"

#include "Blueprint/UserWidget.h"

void AUERoadmapHUD::ShowMainMenu()
{
	// Make widget owned by our PlayerController
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	MainMenu = CreateWidget<UUserWidget>(PC, MainMenuClass);

	MainMenu->AddToViewport();
}

void AUERoadmapHUD::HideMainMenu()
{
	if (MainMenu)
	{
		MainMenu->RemoveFromParent();
		MainMenu = nullptr;
	}
}