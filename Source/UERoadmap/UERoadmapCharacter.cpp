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
#include "UERoadmapGameMode.h"
#include "UERoadmapHUD.h"
#include "UERoadmapSaveGame.h"
#include "UERoadmapStreamingManager.h"
#include "UERoadmap_Ammo_AttributeSet.h"
#include "UERoadmap_GA_GravityGun.h"
#include "UERoadmap_GE_GravityGunActivated.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
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

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
	PhysicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
}

void AUERoadmapCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
	
	AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(UUERoadmap_GA_GravityGun::StaticClass(), 1, 0));
	AbilitySystemComponent->AddAttributeSetSubobject(NewObject<UUERoadmap_Ammo_AttributeSet>(this));
}

void AUERoadmapCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (Grenade && bIsThrowingGreanade)
	{
		Grenade->PredictPath(UKismetMathLibrary::GetForwardVector(GetControlRotation()));
	}

	// cheats checks
	CheckNoclipCheat();
	bIsInfiniteAmmoEnabled = ConsoleVars::CVarAmmoCheat.GetValueOnAnyThread();

	if (PhysicsHandle->GrabbedComponent)
	{
		APlayerController* PlayerController = UGameplayStatics::GetPlayerController(GetWorld(), 0);
        const FRotator GrabbedComponentRotation = PlayerController->PlayerCameraManager->GetCameraRotation();
        const FVector GrabbedComponentLocation = PlayerController->PlayerCameraManager->GetCameraLocation() + GrabbedComponentRotation.RotateVector(FVector(200.0, 0.0, 0.0));
		PhysicsHandle->SetTargetLocationAndRotation(GrabbedComponentLocation, GrabbedComponentRotation);
	}

	if (UUERoadmapStreamingManager* StreamingManager = UUERoadmapStreamingManager::Get(this))
	{
		StreamingManager->UpdateStreaming(this);
	}

	if (GravityGunEnergyGameplayEffect.IsValid())
	{
		float GravityGunEnergyVal = AbilitySystemComponent->GetNumericAttribute(UUERoadmap_Ammo_AttributeSet::GetGravityGunEnergyAttribute());
		FString DebugText = FString::Printf(TEXT("GravityGunEnergy: %.0f"), GravityGunEnergyVal);
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Green, DebugText);
	}
}

UAbilitySystemComponent* AUERoadmapCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
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
        
        // Throw grenade
		EnhancedInputComponent->BindAction(ThrowGrenadeAction, ETriggerEvent::Started, this, &AUERoadmapCharacter::ThrowGrenade);
		EnhancedInputComponent->BindAction(ThrowGrenadeAction, ETriggerEvent::Completed, this, &AUERoadmapCharacter::ThrowGrenadeReleased);
		
		// Save/Load
		EnhancedInputComponent->BindAction(SaveGameAction, ETriggerEvent::Triggered, this, &AUERoadmapCharacter::SaveGameOnInput);
		EnhancedInputComponent->BindAction(LoadGameAction, ETriggerEvent::Triggered, this, &AUERoadmapCharacter::LoadGameOnInput);

		// Alternative fire
		EnhancedInputComponent->BindAction(AlternativeFireAction, ETriggerEvent::Completed, this, &AUERoadmapCharacter::ActivateGravityGun);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input Component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AUERoadmapCharacter::CheckNoclipCheat()
{
	bool NoclipEnabled = ConsoleVars::CVarNoclipCheat.GetValueOnAnyThread();

	if (NoclipEnabled != bIsNoclipEnabled)
	{
		UCharacterMovementComponent* MyCharacterMovement = GetCharacterMovement();
		bIsNoclipEnabled = NoclipEnabled;
		MyCharacterMovement->SetMovementMode(bIsNoclipEnabled ? EMovementMode::MOVE_Flying : EMovementMode::MOVE_Walking);
		SetActorEnableCollision(!bIsNoclipEnabled);
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, bIsNoclipEnabled ? TEXT("Noclip enabled") : TEXT("Noclip disabled"));
	}
}


bool AUERoadmapCharacter::OnFireTriggered()
{
	bool bWasFired = false;
	const FGameplayAttribute& GASAmmo = UUERoadmap_Ammo_AttributeSet::GetAmmoAttribute();
	if (!PhysicsHandle->GrabbedComponent && (bIsInfiniteAmmoEnabled || AbilitySystemComponent->GetNumericAttributeChecked(GASAmmo) > 0))
	{
		if (!bIsInfiniteAmmoEnabled)
		{
			if (AmmoModifierGameplayEffectClass)
			{
				FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(AmmoModifierGameplayEffectClass, 1.0f, AbilitySystemComponent->MakeEffectContext());
				AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*Spec.Data, AbilitySystemComponent);
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Current GAS ammo: " + FString::SanitizeFloat(AbilitySystemComponent->GetNumericAttributeChecked(GASAmmo))));
			}
		}
		bWasFired = true;
	}
	return bWasFired;
}

void AUERoadmapCharacter::Move(const FInputActionValue& Value)
{
	FVector MovementVector = Value.Get<FVector>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(bIsNoclipEnabled ? FirstPersonCameraComponent->GetForwardVector() : GetActorForwardVector(), MovementVector.Y);
		if (bIsNoclipEnabled)
		{
			AddMovementInput(FirstPersonCameraComponent->GetForwardVector(), MovementVector.Z);
		}
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

void AUERoadmapCharacter::SaveGameOnInput(const FInputActionValue& Value)
{
	UUERoadmapSaveGame* SaveGameInstance = Cast<UUERoadmapSaveGame>(UGameplayStatics::CreateSaveGameObject(UUERoadmapSaveGame::StaticClass()));

	if (SaveGameInstance)
	{
		SaveGameInstance->PlayerLocation = GetActorLocation();
		SaveGameInstance->PlayerRotation = GetActorRotation();

		AUERoadmapGameMode* GameMode = Cast<AUERoadmapGameMode>(GetWorld()->GetAuthGameMode());
		GameMode->SaveGameInGameMode(SaveGameInstance);
		
		UGameplayStatics::SaveGameToSlot(SaveGameInstance, SaveGameInstance->SaveSlotName, SaveGameInstance->UserIndex);

		UE_LOG(LogTemp, Log, TEXT("Game Saved!"));
	}
}

void AUERoadmapCharacter::LoadGameOnInput(const FInputActionValue& Value)
{
	UUERoadmapSaveGame* LoadGameInstance = Cast<UUERoadmapSaveGame>(UGameplayStatics::LoadGameFromSlot(TEXT("DefaultSaveSlot"), 0));

	if (LoadGameInstance)
	{
		SetActorLocation(LoadGameInstance->PlayerLocation);
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (PC)
		{
			PC->SetControlRotation(LoadGameInstance->PlayerRotation);
		}
		
		AUERoadmapGameMode* GameMode = Cast<AUERoadmapGameMode>(GetWorld()->GetAuthGameMode());
		GameMode->LoadGameInGameMode(LoadGameInstance);

		UE_LOG(LogTemp, Log, TEXT("Game Loaded!"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("No save game found!"));
	}
}

void AUERoadmapCharacter::ActivateGravityGun(const FInputActionValue& Value)
{
	if (AbilitySystemComponent)
	{
		FGameplayAbilitySpec* Spec = AbilitySystemComponent->FindAbilitySpecFromClass(UUERoadmap_GA_GravityGun::StaticClass());
		if (Spec && Spec->IsActive())
		{
			AbilitySystemComponent->CancelAbilityHandle(Spec->Handle);
			if (GravityGunEnergyGameplayEffect.IsValid())
			{
				AbilitySystemComponent->RemoveActiveGameplayEffect(GravityGunEnergyGameplayEffect);
				AbilitySystemComponent->SetNumericAttributeBase(UUERoadmap_Ammo_AttributeSet::GetGravityGunEnergyAttribute(), 1.0f);
				GravityGunEnergyGameplayEffect.Invalidate();
			}
		}
		else
		{
			if (AbilitySystemComponent->TryActivateAbilityByClass(UUERoadmap_GA_GravityGun::StaticClass()))
			{
				GravityGunEnergyGameplayEffect = AbilitySystemComponent->ApplyGameplayEffectToSelf(UUERoadmap_GE_GravityGunActivated::StaticClass()->GetDefaultObject<UGameplayEffect>(), 1.0f, AbilitySystemComponent->MakeEffectContext());
			}
		}
	}
}

