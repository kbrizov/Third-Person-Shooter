// Fill out your copyright notice in the Description page of Project Settings.

#include "CharacterLocomotionComponent.h"

UCharacterLocomotionComponent::UCharacterLocomotionComponent(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer),
	CurrentState(nullptr),
	PreviousState(nullptr),
	WalkSpeed(270.0f),
	RunSpeed(450.0f),
	CoverSpeed(230.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCharacterLocomotionComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	this->CurrentState->Update();
}

void UCharacterLocomotionComponent::ToCoverState()
{
	this->ToState(this->CoverState);
}

void UCharacterLocomotionComponent::ToIdleState()
{
	this->ToState(this->IdleState);
}

void UCharacterLocomotionComponent::ToWalkState()
{
	this->ToState(this->WalkState);
}

void UCharacterLocomotionComponent::ToRunState()
{
	this->ToState(this->RunState);
}

void UCharacterLocomotionComponent::ToPreviousState()
{
	this->ToState(*PreviousState);
}

void UCharacterLocomotionComponent::OnEnterCover(bool bIsFacingCover)
{
	this->EnterCoverEvent.Broadcast(bIsFacingCover);
}

void UCharacterLocomotionComponent::OnExitCover()
{
	this->ExitCoverEvent.Broadcast();
}

void UCharacterLocomotionComponent::BeginPlay()
{
	Super::BeginPlay();

	this->CoverState = FCoverState(this);
	this->IdleState = FIdleState(this);
	this->WalkState = FWalkState(this);
	this->RunState = FRunState(this);

	this->CurrentState = &(this->IdleState);
	this->PreviousState = nullptr;
}

void UCharacterLocomotionComponent::ToState(FBaseCharacterState& State)
{
	this->CurrentState->OnExit();
	this->CurrentState = &State;
	this->CurrentState->OnEnter();
}
