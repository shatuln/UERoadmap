// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeActor.generated.h"

UCLASS()
class UEROADMAP_API ACubeActor : public AActor
{
	GENERATED_BODY()
	
public:
	/** static mesh component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch Components")
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, Category = "Health", meta=(ClampMin=1, ClampMax=100, EditCondition="!bIsImmortal"), SaveGame)
	int32 HealthPoints;

	UPROPERTY(EditAnywhere, Category = "Health")
	bool bIsImmortal;
	
	// Sets default values for this actor's properties
	ACubeActor();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UFUNCTION()
	bool IsDead();
};
