// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UERoadmapHUD.generated.h"

/**
 * 
 */
UCLASS()
class UEROADMAP_API AUERoadmapHUD : public AHUD
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void ShowMainMenu();

	UFUNCTION()
	void HideMainMenu();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<class UUserWidget> MainMenuClass;
	
protected:

	// Keep a pointer to be able to hide it
	UPROPERTY()
	TObjectPtr<UUserWidget> MainMenu;
	
};
