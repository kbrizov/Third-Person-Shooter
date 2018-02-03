// Fill out your copyright notice in the Description page of Project Settings.

#include "AssaultRifle.h"
#include "TimerManager.h"

AAssaultRifle::AAssaultRifle(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->SetRange(100.0f);
	this->SetRecoil(0.5f);
	this->SetRateOfFire(10);
	this->SetMagazineCapacity(50);
	this->SetRemainingAmmunition(this->GetMagazineCapacity());
}

void AAssaultRifle::StartFire()
{
	Super::StartFire();

	this->GetWorldTimerManager().SetTimer(FireRateTimerHandle, this, &AAssaultRifle::Fire, 1.0f / this->RateOfFire, true, 0.0f);
}

void AAssaultRifle::StopFire()
{
	Super::StopFire();

	this->GetWorldTimerManager().ClearTimer(this->FireRateTimerHandle);
}

int32 AAssaultRifle::GetRateOfFire() const
{
	return this->RateOfFire;
}

void AAssaultRifle::SetRateOfFire(int32 Value)
{
	this->RateOfFire = Value < 0 ? 0 : Value;
}
