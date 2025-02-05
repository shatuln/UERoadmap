// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmap_GA_GravityGun.h"

#include "GameFramework/Character.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"

UUERoadmap_GA_GravityGun::UUERoadmap_GA_GravityGun()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UUERoadmap_GA_GravityGun::CanActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags,
	const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const
{
	return true;
}

void UUERoadmap_GA_GravityGun::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (HeldActor) 
	{
		ThrowObject();
	}
	else 
	{
		AActor* Target = FindTargetActor();
		if (Target) 
		{
			PickupObject(Target);
		}
	}
}

void UUERoadmap_GA_GravityGun::CancelAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateCancelAbility)
{
	DropObject();
	Super::CancelAbility(Handle, ActorInfo, ActivationInfo, bReplicateCancelAbility);
}

AActor* UUERoadmap_GA_GravityGun::FindTargetActor()
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
		return Hit.GetActor();
	}

	return nullptr;
}

void UUERoadmap_GA_GravityGun::PickupObject(AActor* Target)
{if (!Target) return;

	HeldActor = Target;

	// Добавляем Physics Handle к владельцу
	if (!PhysicsHandle)
	{
		PhysicsHandle = NewObject<UPhysicsHandleComponent>(GetAvatarActorFromActorInfo());
		PhysicsHandle->RegisterComponent();
	}

	UPrimitiveComponent* TargetComponent = Cast<UPrimitiveComponent>(Target->GetRootComponent());
	if (TargetComponent)
	{
		TargetComponent->SetSimulatePhysics(true);
		PhysicsHandle->GrabComponentAtLocationWithRotation(TargetComponent, NAME_None, Target->GetActorLocation(), Target->GetActorRotation());
	}
}

void UUERoadmap_GA_GravityGun::DropObject()
{
	if (PhysicsHandle && HeldActor)
	{
		PhysicsHandle->ReleaseComponent();
		HeldActor = nullptr;
	}
}

void UUERoadmap_GA_GravityGun::ThrowObject()
{
	if (PhysicsHandle && HeldActor)
	{
		UPrimitiveComponent* TargetComponent = Cast<UPrimitiveComponent>(HeldActor->GetRootComponent());
		if (TargetComponent)
		{
			TargetComponent->AddImpulse(GetAvatarActorFromActorInfo()->GetActorForwardVector() * ThrowForce, NAME_None, true);
		}

		DropObject();
	}
}


