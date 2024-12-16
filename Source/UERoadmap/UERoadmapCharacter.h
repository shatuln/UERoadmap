// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "UERoadmapGrenade.h"
#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "UERoadmapCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class UInputAction;
class UInputMappingContext;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

namespace ConsoleVars
{
	static TAutoConsoleVariable<bool> CVarNoclipCheat(
		TEXT("cheat.Noclip"),
		0,
		TEXT("Noclip cheat code.\n")
		TEXT("0: off\n")
		TEXT(" 1: on\n"),
		ECVF_Cheat);
	
	static TAutoConsoleVariable<bool> CVarAmmoCheat(
		TEXT("cheat.Ammo"),
		0,
		TEXT("Infinite ammo cheat code.\n")
		TEXT("0: off\n")
		TEXT(" 1: on\n"),
		ECVF_Cheat);
}

UCLASS(config=Game)
class AUERoadmapCharacter : public ACharacter
{
	GENERATED_BODY()

	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* MoveAction;
	
	/** Toggle Menu Action */
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ToggleMenuAction;

	/** Throw Grenade Action */
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* ThrowGrenadeAction;
	
	/** Save/Load Action */
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* SaveGameAction;
	UPROPERTY(EditAnywhere, Category=Input, meta=(AllowPrivateAccess = "true"))
	UInputAction* LoadGameAction;

	/** Ammo count */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ammo,  meta = (AllowPrivateAccess = "true"))
	int32 AmmoCount;

	bool bIsMenuToggled = false;
	
public:
	AUERoadmapCharacter();

protected:
	virtual void BeginPlay();
	
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditDefaultsOnly, Category = Grenade)
	TSubclassOf<class AUERoadmapGrenade> GrenadeClass;
	
	UPROPERTY(EditAnywhere, Category = Grenade, meta = (AllowPrivateAccess = "true"))
	class AUERoadmapGrenade* Grenade;
	
public:
		
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UInputAction* LookAction;
	
	UFUNCTION()
	bool OnFireTriggered();

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for ToggleMenu input */
	void ToggleMenu(const FInputActionValue& Value);

	void ThrowGrenade(const FInputActionValue& Value);
	void ThrowGrenadeReleased(const FInputActionValue& Value);

	void SaveGameOnInput(const FInputActionValue& Value);
	void LoadGameOnInput(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

private:
	bool bIsThrowingGreanade = false;

	bool bIsNoclipEnabled = false;
	bool bIsInfiniteAmmoEnabled = false;

	void CheckNoclipCheat();
};

