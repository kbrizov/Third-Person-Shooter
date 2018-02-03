// Fill out your copyright notice in the Description page of Project Settings.

#include "WalkState.h"
#include "BaseCharacter.h"
#include "CharacterLocomotionComponent.h"

FWalkState::FWalkState() 
	: FWalkState(nullptr)
{
}

FWalkState::FWalkState(UCharacterLocomotionComponent* LocomotionComponent) 
	: FBaseCharacterState(LocomotionComponent)
{
}

void FWalkState::OnEnter()
{
	FBaseCharacterState::OnEnter();

	this->LocomotionComponent->MaxWalkSpeed = this->LocomotionComponent->WalkSpeed;
}

void FWalkState::Update()
{
	// To idle state.
	if (Character->IsIdle())
	{
		this->LocomotionComponent->ToIdleState();

		return;
	}

	// To run state.
	if (Character->IsRunning())
	{
		this->LocomotionComponent->ToRunState();

		return;
	}

	FBaseCharacterState::Update();
}

FString FWalkState::ToString() const
{
	return FString(TEXT("Walk State"));
}
