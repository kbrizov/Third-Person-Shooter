// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Animation/AnimInstance.h>
#include "BaseWeaponAnimInstance.generated.h"

class ABaseWeapon;

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API UBaseWeaponAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	void NativeInitializeAnimation() override;

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Fire"))
	void OnFire();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Reload"))
	void OnReload();

private:

	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	ABaseWeapon* Weapon;
};
