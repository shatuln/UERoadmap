// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmapGrenade.h"

// Sets default values
AUERoadmapGrenade::AUERoadmapGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshGrenade = CreateDefaultSubobject<UStaticMeshComponent>("MeshGrenade");
	RootComponent = MeshGrenade;

}

// Called when the game starts or when spawned
void AUERoadmapGrenade::BeginPlay()
{
	Super::BeginPlay();
	MeshGrenade->SetSimulatePhysics(false);
}

void AUERoadmapGrenade::OnReleased()
{
	MeshGrenade->SetSimulatePhysics(true);
	MeshGrenade->AddImpulse(FVector(2500.0, 0.0, 500.0));
}
