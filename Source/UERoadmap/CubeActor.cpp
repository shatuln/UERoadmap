// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeActor.h"

#include "UERoadmapProjectile.h"

// Sets default values
ACubeActor::ACubeActor()
{
	State = Invalid;
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

void ACubeActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetCubeState();
	LoadDataTable();
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

void ACubeActor::LoadDataTable()
{
    static const FString ContextString(TEXT("DataTableContext"));
	
    UDataTable* DataTable = LoadObject<UDataTable>(nullptr, TEXT("/Game/Data/CubeActorsData.CubeActorsData"));

    if (DataTable)
    {
        FName RowName;
    	switch (State)
    	{
    	case Immortal:
    		RowName = FName("Immortal");
    		break;
    	case FullHP:
    		RowName = FName("FullHP");
    		break;
    	case HalfHP:
    		RowName = FName("HalfHP");
    		break;
    	case LowHP:
    		RowName = FName("LowHP");
    		break;
    	default:
    		UE_LOG(LogTemp, Error, TEXT("Incorrect state handled for Cube data table"));
    		break;
    	}
    	
        FCubeActorData* Row = DataTable->FindRow<FCubeActorData>(RowName, ContextString);
        if (Row)
        {
        	CubeMaterial = Row->CubeMaterial;
        	StaticMeshComponent->SetWorldScale3D(FVector(Row->ScalePercentage, Row->ScalePercentage, Row->ScalePercentage));
        	
        	UMaterialInterface* NewMaterial = LoadObject<UMaterialInterface>(nullptr, *CubeMaterial);
        	if (NewMaterial)
        	{
        		StaticMeshComponent->SetMaterial(0, NewMaterial);
        		StaticMeshComponent->MarkRenderStateDirty();
        	}
        	else
        	{
        		UE_LOG(LogTemp, Error, TEXT("Failed to load material %s"), *CubeMaterial);
        	}
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Row not found in DataTable!"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("DataTable not found!"));
    }
}

void ACubeActor::SetCubeState()
{
	if (bIsImmortal)
	{
		State = Immortal;
	}
	else if (HealthPoints == 100)
	{
		State = FullHP;
	}
	else if (HealthPoints >= 50)
	{
		State = HalfHP;
	}
	else if (HealthPoints < 50)
	{
		State = LowHP;
	}
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
			if (!IsDead())
			{
				CubeState CurrentState = State;
				SetCubeState();
				if (CurrentState != State)
				{
					LoadDataTable();
				}
			}
		}
	}
}

