// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/BasePlayerWidget.h"
#include "WeaponWidget.generated.h"

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API UWeaponWidget : public UBasePlayerWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FText GetRemainingAmmunition() const;

	UFUNCTION(BlueprintCallable, Category = "Weapon")
	FText GetAvailableAmmunition() const;
};
