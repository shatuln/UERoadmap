// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmap_Ammo_AttributeSet.h"

void UUERoadmap_Ammo_AttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	if (Attribute == GetGravityGunEnergyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 1.0f, 100.0f);
	}
}
