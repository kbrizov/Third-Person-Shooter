// Fill out your copyright notice in the Description page of Project Settings.

#include "WeaponWidget.h"
#include "PlayerCharacter.h"
#include "BaseWeapon.h"

FText UWeaponWidget::GetRemainingAmmunition() const
{
	APlayerCharacter* PlayerCharacter = this->GetPlayerCharacter();
	FString Result = FString(TEXT("0"));

	if (PlayerCharacter)
	{
		ABaseWeapon* EquipedWeapon = PlayerCharacter->GetEquipedWeapon();

		if (EquipedWeapon)
		{
			int32 RemainingAmmo = EquipedWeapon->GetRemainingAmmunition();
			Result = FString::FromInt(RemainingAmmo).LeftPad(3);
			Result = Result.Replace(TEXT(" "), TEXT("0"));
		}
	}

	return FText::FromString(Result);
}

FText UWeaponWidget::GetAvailableAmmunition() const
{
	// TODO: Implement logic to retrive data from an inventory?
	FString Result = FString(TEXT("INF"));

	return FText::FromString(Result);
}
