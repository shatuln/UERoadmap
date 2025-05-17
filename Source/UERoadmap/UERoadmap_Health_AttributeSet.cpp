// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmap_Health_AttributeSet.h"

void UUERoadmap_Health_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	
	if (ASC && Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, ASC->GetNumericAttribute(GetMaxHealthAttribute()));
	}
}

void UUERoadmap_Health_AttributeSet::PostAttributeChange(const FGameplayAttribute& Attribute, float OldValue, float NewValue)
{
	Super::PostAttributeChange(Attribute, OldValue, NewValue);
	
	UAbilitySystemComponent* ASC = GetOwningAbilitySystemComponent();
	if (ASC)
	{
		if (Attribute == GetHealthAttribute() && NewValue == ASC->GetNumericAttribute(GetMaxHealthAttribute()))
		{
			ASC->RemoveLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Injured")));
		}
		else if (Attribute == GetHealthAttribute() && NewValue <= 0.0f)
		{
			ASC->AddLooseGameplayTag(FGameplayTag::RequestGameplayTag(FName("State.Dead")));
		}
	}
	
}
