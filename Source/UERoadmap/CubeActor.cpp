// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeActor.h"

#include "UERoadmapProjectile.h"

// Sets default values
ACubeActor::ACubeActor()
{
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	HealthPoints = 100;

	StaticMeshComponent->OnComponentHit.AddDynamic(this, &ACubeActor::OnHit);
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	//PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACubeActor::BeginPlay()
{
	Super::BeginPlay();
}

float ACubeActor::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float actualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (!bIsImmortal)
	{
		HealthPoints -= actualDamage;
		IsDead();
	}
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Taken damage " + FString::FromInt(actualDamage) + ", health: " + FString::FromInt(HealthPoints)));
	return actualDamage;
}

// Called every frame
void ACubeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

bool ACubeActor::IsDead()
{
	bool isDead = false;
	if (HealthPoints <= 0)
	{
		isDead = true;
		Destroy();
	}
	return isDead;
}

void ACubeActor::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                       FVector NormalImpulse, const FHitResult& Hit)
{
	if (!bIsImmortal)
	{
		AUERoadmapProjectile* Projectile = Cast<AUERoadmapProjectile>(OtherActor);
		if (Projectile != nullptr && OtherActor != nullptr && OtherActor != this)
		{
			HealthPoints -= 10;
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Hitted " + FString::FromInt(HealthPoints)));
			IsDead();
		}
	}
}

