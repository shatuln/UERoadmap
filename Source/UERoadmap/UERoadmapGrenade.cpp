// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmapGrenade.h"

#include "Chaos/DebugDrawQueue.h"
#include "Kismet/GameplayStatics.h"

#if ENABLE_VISUAL_LOG
void AUERoadmapGrenade::GrabDebugSnapshot(FVisualLogEntry* Snapshot) const
{
	IVisualLoggerDebugSnapshotInterface::GrabDebugSnapshot(Snapshot);
	const int32 CatIndex = Snapshot->Status.AddZeroed();
	FVisualLogStatusCategory& PlaceableCategory = Snapshot->Status[CatIndex];
	PlaceableCategory.Category = TEXT("Grenade Debug");
	PlaceableCategory.Add(TEXT("Grenade Sphere Damage"), FString::Printf(TEXT("%4.2f"), DamageSphereRadius));
}
#endif

// Sets default values
AUERoadmapGrenade::AUERoadmapGrenade()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	FuseLength = 5.0f;
	DamageSphereRadius = 500.0f;

	MeshGrenade = CreateDefaultSubobject<UStaticMeshComponent>("MeshGrenade");
	RootComponent = MeshGrenade;

	SphereDamage = CreateDefaultSubobject<USphereComponent>("SphereDamage");
	SphereDamage->SetSphereRadius(DamageSphereRadius);
	SphereDamage->SetVisibility(true);
	//SphereDamage->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AUERoadmapGrenade::BeginPlay()
{
	Super::BeginPlay();
	
	MeshGrenade->SetSimulatePhysics(false);
	MeshGrenade->SetCollisionEnabled(ECollisionEnabled::Type::NoCollision);
}

float AUERoadmapGrenade::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	Explode();
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

void AUERoadmapGrenade::Explode()
{
	if (!bIsExploding)
	{
		//TBD: why this is not working????
		//TArray<AActor*> Overlaps;
		//SphereDamage->GetOverlappingActors(Overlaps);

		bIsExploding = true;
		TArray<FHitResult> OutHits;
		FCollisionShape MyColSphere = FCollisionShape::MakeSphere(DamageSphereRadius);
		UE_VLOG_LOCATION(this, LogTemp, Verbose, GetActorLocation(), DamageSphereRadius, FColor::Green, TEXT("Grenade damage sphere"));
		bool isHit = GetWorld()->SweepMultiByChannel(OutHits, GetActorLocation(), GetActorLocation(), FQuat::Identity, ECC_PhysicsBody, MyColSphere);

		if (isHit)
		{
			for (auto& Hit : OutHits)
			{
				UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>((Hit.GetActor())->GetRootComponent());
				if (MeshComp)
				{
					UE_VLOG_ARROW(Hit.GetActor(), LogTemp, Verbose, GetActorLocation(), Hit.ImpactPoint, FColor::Red, TEXT("Grenade hit vector"));
					UE_VLOG_LOCATION(Hit.GetActor(), LogTemp, Verbose, Hit.ImpactPoint, 5.0, FColor::Red, TEXT("Grenade hit location"));
					MeshComp->AddRadialImpulse(GetActorLocation(), DamageSphereRadius, 1000.f, ERadialImpulseFalloff::RIF_Constant, true);
				}
			}
		}

		UGameplayStatics::ApplyRadialDamage(GetWorld(),	50.0f, GetActorLocation(), DamageSphereRadius, UDamageType::StaticClass(), TArray<AActor*>(), this, nullptr, false, ECollisionChannel::ECC_Visibility);

		Destroy();
	}
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
	
	UE_VLOG_ARROW(this, LogTemp, Verbose, GetActorLocation(), FVector::ForwardVector + ForwardVector, FColor::Green, TEXT("Grenade launch vector"));
}

void AUERoadmapGrenade::PredictPath(FVector ForwardVector)
{
	ForwardVector *= 2500.0;
	ForwardVector.Z += 250.0;
	
	FPredictProjectilePathParams PredictParams;
	PredictParams.StartLocation = GetActorLocation();
	PredictParams.LaunchVelocity = ForwardVector;
	PredictParams.MaxSimTime = 2.0f;
	PredictParams.DrawDebugType = EDrawDebugTrace::ForOneFrame;
	PredictParams.bTraceWithCollision = true;
	PredictParams.bTraceComplex = true;
	PredictParams.SimFrequency = 30.0f;

	FPredictProjectilePathResult PredictResult;
	UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);
}
