// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "VisualLogger/VisualLoggerDebugSnapshotInterface.h"
#include "UERoadmapSaveGame.h"
#include "UERoadmapGrenade.generated.h"

UCLASS()
class UEROADMAP_API AUERoadmapGrenade : public AActor, public IVisualLoggerDebugSnapshotInterface
{
	GENERATED_BODY()

#if ENABLE_VISUAL_LOG
	virtual void GrabDebugSnapshot(FVisualLogEntry* Snapshot) const override;
#endif
	
public:	
	// Sets default values for this actor's properties
	AUERoadmapGrenade();
	
	UPROPERTY(EditAnywhere, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* MeshGrenade;

	UPROPERTY(EditAnywhere)
	USphereComponent* SphereDamage;

	UPROPERTY(EditAnywhere)
	float FuseLength;

	UPROPERTY(EditAnywhere)
	float DamageSphereRadius;

	UPROPERTY(EditAnywhere)
	FTimerHandle ExplodeTimerHandle;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	void Explode();
	
public:
	void OnReleased(FVector ForwardVector);

	void PredictPath(FVector ForwardVector);

	void SaveGameOnGrenade(FActorSaveData& SaveGameActorData);
	void LoadGameOnGrenade(const FActorSaveData& LoadGameActorData);
	
private:
	bool bIsExploding = false;
};
