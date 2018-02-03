// Fill out your copyright notice in the Description page of Project Settings.

#include "ElasticArmComponent.h"

UElasticArmComponent::UElasticArmComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->bShouldUseMinArmLength = false;
	this->bShouldUseMaxArmLength = true;

	this->MinArmLength = 80.0f;
	this->MaxArmLength = 160.0f;

	this->ArmLengthChangeSpeed = 20.0f;
}

void UElasticArmComponent::UseMinArmLength()
{
	this->bShouldUseMinArmLength = true;
	this->bShouldUseMaxArmLength = false;
}

void UElasticArmComponent::UseMaxArmLength()
{
	this->bShouldUseMinArmLength = false;
	this->bShouldUseMaxArmLength = true;
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
