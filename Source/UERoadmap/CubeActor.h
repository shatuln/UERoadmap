// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CubeActor.generated.h"

USTRUCT(BlueprintType)
struct FCubeActorData : public FTableRowBase
	{
		GENERATED_USTRUCT_BODY()

	public:

		FCubeActorData()
		: ScalePercentage(1.0f)
		, CubeMaterial("")
		{}

		UPROPERTY(EditAnywhere)
		float ScalePercentage;

		UPROPERTY(EditAnywhere)
		FString CubeMaterial;
	};

UCLASS()
class UEROADMAP_API ACubeActor : public AActor
{
	GENERATED_BODY()
	
public:
	/** static mesh component */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Switch Components")
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta=(ClampMin=1, ClampMax=100, EditCondition="!bIsImmortal"), SaveGame)
	int32 SuperNewHealthPoints;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
	bool bIsImmortal;

	UPROPERTY(EditAnywhere, Category = "Misc")
	bool bSuppressMaterialChange;

	UPROPERTY(VisibleAnywhere, Category = "Visual", SaveGame)
	FString CubeMaterial;
	
	// Sets default values for this actor's properties
	ACubeActor();

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
	
	virtual void OnConstruction(const FTransform& Transform) override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	enum CubeState
	{
		Invalid = -1,
		Immortal,
		FullHP,
		HalfHP,
		LowHP
	};
	
	UFUNCTION()
	bool IsDead();

	void LoadDataTable();
	void SetCubeState();

	FTransform DefaultTransform;
	CubeState State;
};
