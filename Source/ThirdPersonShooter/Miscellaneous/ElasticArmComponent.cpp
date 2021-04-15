// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonShooter/Miscellaneous/ElasticArmComponent.h"

UElasticArmComponent::UElasticArmComponent(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer),
	bShouldUseMinArmLength(false),
	bShouldUseMaxArmLength(true),
	MinArmLength(80.0f),
	MaxArmLength(160.0f),
	ArmLengthChangeSpeed(20.0f)
{
}

void UElasticArmComponent::UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime)
{
	if (bShouldUseMinArmLength)
	{
		this->TargetArmLength = FMath::FInterpTo(this->TargetArmLength, this->MinArmLength, DeltaTime, this->ArmLengthChangeSpeed);
	}
	else if (bShouldUseMaxArmLength)
	{
		this->TargetArmLength = FMath::FInterpTo(this->TargetArmLength, this->MaxArmLength, DeltaTime, this->ArmLengthChangeSpeed);
	}

	Super::UpdateDesiredArmLocation(bDoTrace, bDoLocationLag, bDoRotationLag, DeltaTime);
}
