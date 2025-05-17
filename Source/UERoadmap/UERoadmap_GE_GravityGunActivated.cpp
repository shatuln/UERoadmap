// Fill out your copyright notice in the Description page of Project Settings.


#include "UERoadmap_GE_GravityGunActivated.h"

#include "UERoadmap_Ammo_AttributeSet.h"

UUERoadmap_GE_GravityGunActivated::UUERoadmap_GE_GravityGunActivated()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;
	Period.Value = 0.1f;

	FGameplayModifierInfo ModifierInfo;
	FProperty* EnergyProperty = FindFieldChecked<FProperty>(UUERoadmap_Ammo_AttributeSet::StaticClass(), GET_MEMBER_NAME_CHECKED(UUERoadmap_Ammo_AttributeSet, GravityGunEnergy));
	ModifierInfo.Attribute = FGameplayAttribute(EnergyProperty);
	ModifierInfo.ModifierOp = EGameplayModOp::Additive;
	ModifierInfo.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(1.5f));
	Modifiers.Add(ModifierInfo);
	bExecutePeriodicEffectOnApplication = true;
}
