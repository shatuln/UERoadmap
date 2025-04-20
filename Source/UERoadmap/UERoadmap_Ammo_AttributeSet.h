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
	
public:
	
	UPROPERTY(BlueprintReadOnly, Category = "Ammo", SaveGame)
	FGameplayAttributeData Ammo;
	ATTRIBUTE_ACCESSORS(UUERoadmap_Ammo_AttributeSet, Ammo);
	
};
