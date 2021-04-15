// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonShooter/Weapons/BaseWeaponAnimInstance.h"
#include "ThirdPersonShooter/Weapons/BaseWeapon.h"

void UBaseWeaponAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	this->Weapon = Cast<ABaseWeapon>(this->GetOwningActor());

	if (Weapon)
	{
		this->Weapon->FireEvent.AddUFunction(this, "OnFire");
		this->Weapon->ReloadEvent.AddUFunction(this, "OnReload");
	}
}
