// Fill out your copyright notice in the Description page of Project Settings.


#include "CubeActor.h"

#include "AbilitySystemComponent.h"
#include "GameplayEffectTypes.h"
#include "UERoadmapProjectile.h"
#include "UERoadmap_Health_AttributeSet.h"

// Sets default values
ACubeActor::ACubeActor()
{
	State = Invalid;
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(RootComponent);

	SuperNewHealthPoints = 100;

	bSuppressMaterialChange = false;

	StaticMeshComponent->OnComponentHit.AddDynamic(this, &ACubeActor::OnHit);

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ACubeActor::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->AddAttributeSetSubobject(NewObject<UUERoadmap_Health_AttributeSet>(this));
	AbilitySystemComponent->SetNumericAttributeBase(UUERoadmap_Health_AttributeSet::GetHealthAttribute(), SuperNewHealthPoints);
}

float ACubeActor::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	float actualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);

	if (!bIsImmortal)
	{
		FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(HealthHitGameplayEffectClass, 1.0f, AbilitySystemComponent->MakeEffectContext());
		Spec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.HealthHit")), -actualDamage);
		AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*Spec.Data, AbilitySystemComponent);
			
		AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Injured")));
		IsDead();
	}
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Taken damage " + FString::FromInt(actualDamage) + ", health: " + FString::FromInt(SuperNewHealthPoints)));
	return actualDamage;
}

void ACubeActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	SetCubeState();
	LoadDataTable();
}

UAbilitySystemComponent* ACubeActor::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called every frame
void ACubeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HealthRegenGameplayEffectHandle.IsValid())
	{
		const float HealthVal = AbilitySystemComponent->GetNumericAttribute(UUERoadmap_Health_AttributeSet::GetHealthAttribute());
		FString DebugText = FString::Printf(TEXT("Cube health regen: %.0f"), HealthVal);
		GEngine->AddOnScreenDebugMessage(1, 0.f, FColor::Green, DebugText);
		
		if (!AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Injured"))))
		{
			AbilitySystemComponent->RemoveActiveGameplayEffect(HealthRegenGameplayEffectHandle);
			HealthRegenGameplayEffectHandle.Invalidate();
		}
	}

}

bool ACubeActor::IsDead()
{
	bool isDead = false;
	if (AbilitySystemComponent->HasMatchingGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dead"))))
	{
		isDead = true;
		//Destroy();
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
	float HealthVal = 0.0f;
	if (GIsEditor && !GetWorld()->IsGameWorld())
	{
		HealthVal = SuperNewHealthPoints;
	}
	else
	{
		HealthVal = AbilitySystemComponent->GetNumericAttribute(UUERoadmap_Health_AttributeSet::GetHealthAttribute());
	}
	
	if (bIsImmortal)
	{
		State = Immortal;
	}
	else if (HealthVal == 100)
	{
		State = FullHP;
	}
	else if (HealthVal >= 50)
	{
		State = HalfHP;
	}
	else if (HealthVal < 50)
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
			if (!HealthRegenGameplayEffectHandle.IsValid())
			{
				FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(HealthRegenGameplayEffectClass, 1.0f, AbilitySystemComponent->MakeEffectContext());
				HealthRegenGameplayEffectHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*Spec.Data, AbilitySystemComponent);
			}
			
			FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(HealthHitGameplayEffectClass, 1.0f, AbilitySystemComponent->MakeEffectContext());
			Spec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.HealthHit")), -20.0f);
			AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*Spec.Data, AbilitySystemComponent);
			
			AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Injured")));
			
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Hitted " + FString::FromInt(AbilitySystemComponent->GetNumericAttribute(UUERoadmap_Health_AttributeSet::GetHealthAttribute()))));
			if (!IsDead())
			{
				CubeState CurrentState = State;
				SetCubeState();
				if (!bSuppressMaterialChange && CurrentState != State)
				{
					LoadDataTable();
				}
			}
		}
	}
}

