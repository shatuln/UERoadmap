// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmap_EnemyCharacter.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemComponent.h"
#include "ActiveGameplayEffectHandle.h"
#include "UERoadmap_Health_AttributeSet.h"

// Sets default values
AUERoadmap_EnemyCharacter::AUERoadmap_EnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));

}

void AUERoadmap_EnemyCharacter::OnHit(float HitPoints)
{
	FGameplayEffectSpecHandle Spec = AbilitySystemComponent->MakeOutgoingSpec(HealthHitGameplayEffectClass, 1.0f, AbilitySystemComponent->MakeEffectContext());
	Spec.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.HealthHit")), HitPoints);
	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*Spec.Data, AbilitySystemComponent);
			
	AbilitySystemComponent->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Injured")));
			
	GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Hitted " + FString::FromInt(AbilitySystemComponent->GetNumericAttribute(UUERoadmap_Health_AttributeSet::GetHealthAttribute()))));
}

// Called when the game starts or when spawned
void AUERoadmap_EnemyCharacter::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystemComponent->AddAttributeSetSubobject(NewObject<UUERoadmap_Health_AttributeSet>(this));
	AbilitySystemComponent->SetNumericAttributeBase(UUERoadmap_Health_AttributeSet::GetHealthAttribute(), SuperNewHealthPoints);
}

UAbilitySystemComponent* AUERoadmap_EnemyCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

// Called every frame
void AUERoadmap_EnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AUERoadmap_EnemyCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

