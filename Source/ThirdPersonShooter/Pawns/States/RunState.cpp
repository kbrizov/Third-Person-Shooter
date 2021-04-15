// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonShooter/Pawns/States/RunState.h"
#include "ThirdPersonShooter/Pawns/BaseCharacter.h"
#include "ThirdPersonShooter/Pawns/CharacterLocomotionComponent.h"

FRunState::FRunState()
	: FRunState(nullptr)
{
}

FRunState::FRunState(UCharacterLocomotionComponent* LocomotionComponent)
	: FBaseCharacterState(LocomotionComponent)
{
}

void FRunState::OnEnter()
{
	FBaseCharacterState::OnEnter();

	this->LocomotionComponent->MaxWalkSpeed = this->LocomotionComponent->RunSpeed;
}

void FRunState::Update()
{
	// To idle state.
	if (Character->IsIdle())
	{
		this->LocomotionComponent->ToIdleState();

		return;
	}

	// To walk state.
	if (Character->IsWalking())
	{
		this->LocomotionComponent->ToWalkState();

		return;
	}

	FBaseCharacterState::Update();
}

FString FRunState::ToString() const
{
	return FString(TEXT("Run State"));
}
