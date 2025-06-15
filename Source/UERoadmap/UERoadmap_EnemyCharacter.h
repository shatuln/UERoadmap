// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/Character.h"
#include "UERoadmap_EnemyCharacter.generated.h"

UCLASS()
class UEROADMAP_API AUERoadmap_EnemyCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AUERoadmap_EnemyCharacter();

	UFUNCTION(BlueprintCallable)
	void OnHit(float HitPoints);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta=(ClampMin=1, ClampMax=100, EditCondition="!bIsImmortal"), SaveGame)
	int32 SuperNewHealthPoints;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Abilities")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayEffect")
	TSubclassOf<UGameplayEffect> HealthHitGameplayEffectClass;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
