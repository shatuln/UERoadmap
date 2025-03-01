// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "UERoadmap_GA_GravityGun.generated.h"

/**
 * 
 */
UCLASS()
class UEROADMAP_API UUERoadmap_GA_GravityGun : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UUERoadmap_GA_GravityGun();

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
									const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, 
									FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
								 const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void CancelAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
							   const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateCancelAbility) override;

protected:
	AActor* FindTargetActor() const;
	
	void PickupObject(AActor* Target);
	
	void ThrowObject();

private:

	UPROPERTY()
	AActor* HeldActor;

	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun")
	float MaxPickupDistance = 500.0f;

	UPROPERTY(EditDefaultsOnly, Category = "Gravity Gun")
	float ThrowForce = 2000.0f;
};
