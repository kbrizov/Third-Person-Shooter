// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include "ThirdPersonShooter/Weapons/BaseWeapon.h"
#include "AssaultRifle.generated.h"

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API AAssaultRifle : public ABaseWeapon
{
	GENERATED_BODY()

public:

	AAssaultRifle(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void StartFire() override;

	virtual void StopFire() override;

	int32 GetRateOfFire() const;

protected:

	void SetRateOfFire(int32 Value);

private:

	FTimerHandle FireRateTimerHandle;

	/** The rate of fire of the weapon in seconds (RPS). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	int RateOfFire;
};

inline int32 AAssaultRifle::GetRateOfFire() const
{
	return this->RateOfFire;
}

inline void AAssaultRifle::SetRateOfFire(int32 Value)
{
	this->RateOfFire = Value < 0 ? 0 : Value;
}
