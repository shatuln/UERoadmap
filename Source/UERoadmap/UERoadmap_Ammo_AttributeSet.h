// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "UERoadmap_Ammo_AttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * 
 */
UCLASS()
class UEROADMAP_API UUERoadmap_Ammo_AttributeSet : public UAttributeSet
{
	GENERATED_BODY()

	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	
public:
	
	UPROPERTY(BlueprintReadOnly, Category = "Ammo", SaveGame)
	FGameplayAttributeData Ammo = 0.0f;
	ATTRIBUTE_ACCESSORS(UUERoadmap_Ammo_AttributeSet, Ammo);

	UPROPERTY(BlueprintReadOnly, Category = "Ammo")
	FGameplayAttributeData GravityGunEnergy = 1.0f;
	ATTRIBUTE_ACCESSORS(UUERoadmap_Ammo_AttributeSet, GravityGunEnergy);
	
};
