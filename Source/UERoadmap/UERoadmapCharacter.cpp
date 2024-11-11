// Copyright Epic Games, Inc. All Rights Reserved.

#include "UERoadmapCharacter.h"
#include "UERoadmapProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "UERoadmapHUD.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AUERoadmapCharacter

AUERoadmapCharacter::AUERoadmapCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void AUERoadmapCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AUERoadmapCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Grenade && bIsThrowingGreanade)
	{
		Grenade->PredictPath(UKismetMathLibrary::GetForwardVector(GetControlRotation()));
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AUERoadmapCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AUERoadmapCharacter::Move);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AUERoadmapCharacter::Look);
		
		// Toggle Menu
        EnhancedInputComponent->BindAction(ToggleMenuAction, ETriggerEvent::Triggered, this, &AUERoadmapCharacter::ToggleMenu);
        
        // Throw grenade
		EnhancedInputComponent->BindAction(ThrowGrenadeAction, ETriggerEvent::Started, this, &AUERoadmapCharacter::ThrowGrenade);
		EnhancedInputComponent->BindAction(ThrowGrenadeAction, ETriggerEvent::Completed, this, &AUERoadmapCharacter::ThrowGrenadeReleased);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}


bool AUERoadmapCharacter::OnFireTriggered()
{
	bool bWasFired = false;
	if (AmmoCount > 0)
	{
		AmmoCount--;
		bWasFired = true;
	}
	return bWasFired;
}

void AUERoadmapCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AUERoadmapCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AUERoadmapCharacter::ToggleMenu(const FInputActionValue& Value)
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	AUERoadmapHUD* HUD = PC->GetHUD<AUERoadmapHUD>();

	if (PC != nullptr && HUD != nullptr)
	{
		if (!bIsMenuToggled)
		{
			PC->SetInputMode(FInputModeUIOnly());
			PC->SetShowMouseCursor(true);
			HUD->ShowMainMenu();
		}
		else
		{
			PC->SetInputMode(FInputModeGameOnly());
			PC->SetShowMouseCursor(false);
			HUD->HideMainMenu();
		}
	}
}

void AUERoadmapCharacter::ThrowGrenade(const FInputActionValue& Value)
{
	if (GrenadeClass)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		const FRotator SpawnRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
		const FVector SpawnLocation = PlayerController->PlayerCameraManager->GetCameraLocation() + SpawnRotation.RotateVector(FVector(100.0, 50.0, 0.0));
	
		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		//ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		
		Grenade = GetWorld()->SpawnActor<AUERoadmapGrenade>(GrenadeClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
		if (Grenade != nullptr)
		{
			Grenade->MeshGrenade->AttachToComponent(Mesh1P, FAttachmentTransformRules::KeepWorldTransform);
			bIsThrowingGreanade = true;
		}
		
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Thrown grenade"));
	}
}

void AUERoadmapCharacter::ThrowGrenadeReleased(const FInputActionValue& Value)
{
	bIsThrowingGreanade = false;
	Grenade->OnReleased(UKismetMathLibrary::GetForwardVector(GetControlRotation()));
}
