// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "InputAction.h"
#include "UERoadmapPlayerController.generated.h"

class UInputMappingContext;

/**
 *
 */
UCLASS()
class UEROADMAP_API AUERoadmapPlayerController : public APlayerController
{
	GENERATED_BODY()

	void ToggleMenu();

	bool bIsMenuToggled;

public:
	/** Toggle Menu Action */
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ToggleMenuAction;
	
protected:

	/** Input Mapping Context to be used for player input */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputMappingContext* InputMappingContext;

	UFUNCTION()
	void SetupInputComponent() override;

	virtual void BeginPlay() override;
};
