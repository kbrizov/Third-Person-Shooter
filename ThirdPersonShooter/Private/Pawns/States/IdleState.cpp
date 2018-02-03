// Fill out your copyright notice in the Description page of Project Settings.

#include "IdleState.h"
#include "BaseCharacter.h"
#include "CharacterLocomotionComponent.h"

FIdleState::FIdleState()
	: FIdleState(nullptr)
{
}

FIdleState::FIdleState(UCharacterLocomotionComponent* LocomotionComponent)
	: FBaseCharacterState(LocomotionComponent)
{
}

void FIdleState::OnEnter()
{
	FBaseCharacterState::OnEnter();

	this->LocomotionComponent->bUseControllerDesiredRotation = false;
}

void FIdleState::OnExit()
{
	FBaseCharacterState::OnExit();

	this->LocomotionComponent->bUseControllerDesiredRotation = true;
}

void FIdleState::Update()
{
	this->UpdateControllerDesiredRotation();
	
	// To either walking or running state.
	if (!Character->IsIdle())
	{
		if (Character->IsWalking())
		{
			this->LocomotionComponent->ToWalkState();
		}
		else if (Character->IsRunning())
		{
			this->LocomotionComponent->ToRunState();
		}
	}

	FBaseCharacterState::Update();
}

FString FIdleState::ToString() const
{
	return FString(TEXT("Idle State"));
}

void FIdleState::UpdateControllerDesiredRotation()
{
	const FRotator ControlRotation = this->Character->GetControlRotation();
	const FRotator YawRotation = FRotator(0.0f, ControlRotation.Yaw, 0.0f);

	const FVector ControlForward = YawRotation.Vector();
	const FVector ActorForward = this->Character->GetActorForwardVector();

	if (!ControlForward.Equals(ActorForward))
	{
		float DotProduct = FVector::DotProduct(ControlForward, ActorForward);
		float AngleInDegrees = FMath::RadiansToDegrees(FMath::Acos(DotProduct));

		if (AngleInDegrees > 90.0f)
		{
			this->LocomotionComponent->bUseControllerDesiredRotation = true;
		}
	}
	else
	{
		this->LocomotionComponent->bUseControllerDesiredRotation = false;
	}
}
