// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmap_GA_GravityGun.h"

#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "UERoadmapCharacter.h"
#include "Kismet/KismetMathLibrary.h"

UUERoadmap_GA_GravityGun::UUERoadmap_GA_GravityGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UUERoadmap_GA_GravityGun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return FindTargetActor() != nullptr && ActorInfo->AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag("Weapon.Held"));
}

void UUERoadmap_GA_GravityGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	AActor* Target = FindTargetActor();
	if (Target) 
	{
		PickupObject(Target);
	}

	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

void UUERoadmap_GA_GravityGun::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	ThrowObject();
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

AActor* UUERoadmap_GA_GravityGun::FindTargetActor() const
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetAvatarActorFromActorInfo());
	if (!OwnerCharacter) return nullptr;

	FVector Start = OwnerCharacter->GetActorLocation() + OwnerCharacter->GetControlRotation().Vector() * 100.0f;
	FVector End = Start + OwnerCharacter->GetControlRotation().Vector() * MaxPickupDistance;

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(OwnerCharacter);

	if (GetWorld()->LineTraceSingleByChannel(Hit, Start, End, ECC_PhysicsBody, Params))
	{
		if (Hit.GetComponent()->IsSimulatingPhysics())
		{
			return Hit.GetActor();
		}
	}

	return nullptr;
}

void UUERoadmap_GA_GravityGun::PickupObject(AActor* Target)
{
	if (!Target) return;

	AUERoadmapCharacter* OwnerCharacter = Cast<AUERoadmapCharacter>(GetAvatarActorFromActorInfo());
	UPrimitiveComponent* TargetComponent = Cast<UPrimitiveComponent>(Target->GetRootComponent());
	if (TargetComponent && OwnerCharacter->PhysicsHandle)
	{
		HeldActor = Target;
		OwnerCharacter->PhysicsHandle->GrabComponentAtLocationWithRotation(TargetComponent, NAME_None, TargetComponent->GetComponentLocation(), TargetComponent->GetComponentRotation());
	}
}

void UUERoadmap_GA_GravityGun::ThrowObject()
{
	if (HeldActor)
	{
		AUERoadmapCharacter* OwnerCharacter = Cast<AUERoadmapCharacter>(GetAvatarActorFromActorInfo());
		UPrimitiveComponent* TargetComponent = Cast<UPrimitiveComponent>(HeldActor->GetRootComponent());
		if (TargetComponent)
		{
			OwnerCharacter->PhysicsHandle->ReleaseComponent();
			HeldActor = nullptr;
			TargetComponent->AddImpulse(UKismetMathLibrary::GetForwardVector(OwnerCharacter->GetControlRotation()) * ThrowForce, NAME_None, true);
		}
	}
}


