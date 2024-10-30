// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmapGrenade.h"

// Sets default values
AUERoadmapGrenade::AUERoadmapGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FuseLength = 5.0f;
	DamageSphereRadius = 100.0f;

	MeshGrenade = CreateDefaultSubobject<UStaticMeshComponent>("MeshGrenade");
	RootComponent = MeshGrenade;

	SphereDamage = CreateDefaultSubobject<USphereComponent>("SphereDamage");
	SphereDamage->SetSphereRadius(DamageSphereRadius);
	SphereDamage->SetVisibility(true);
	SphereDamage->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AUERoadmapGrenade::BeginPlay()
{
	Super::BeginPlay();
	MeshGrenade->SetSimulatePhysics(false);
	MeshGrenade->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

void AUERoadmapGrenade::Explode()
{
	TArray<AActor*> Overlaps;
	SphereDamage->GetOverlappingActors(Overlaps);

	for (AActor* Actor : Overlaps)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Explode colliding with " + Actor->GetName()));
	}

	Destroy();
}

void AUERoadmapGrenade::OnReleased(FVector ForwardVector)
{
	ForwardVector *= 2500.0;
	ForwardVector.Z += 250.0;

	MeshGrenade->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
	MeshGrenade->SetCollisionEnabled(ECollisionEnabled::Type::QueryAndPhysics);
	MeshGrenade->SetSimulatePhysics(true);
	MeshGrenade->AddImpulse(ForwardVector);

	FTimerHandle ExplodeTimerHandle;
	GetWorldTimerManager().SetTimer(ExplodeTimerHandle, this, &AUERoadmapGrenade::Explode, FuseLength, false);
}
