// Fill out your copyright notice in the Description page of Project Settings.

#include "CoverState.h"
#include "BaseCharacter.h"
#include "CoverBoxComponent.h"
#include "CoverEndComponent.h"
#include "CharacterLocomotionComponent.h"

FCoverState::FCoverState() 
	: FCoverState(nullptr)
{
}

FCoverState::FCoverState(UCharacterLocomotionComponent* LocomotionComponent) 
	: FBaseCharacterState(LocomotionComponent)
{
}

void FCoverState::OnEnter()
{
	FBaseCharacterState::OnEnter();

	UCoverBoxComponent* CoverVolume = this->Character->GetOverlappingCoverVolume();
	FRotator CoverRotation = CoverVolume->GetComponentRotation().GetNormalized();
	this->Character->SetActorRotation(CoverRotation);

	UCoverEndComponent* EndVolume = this->Character->GetOverlappingCoverEndVolume();
	float ControlYawRotation = this->Character->GetControlRotation().GetNormalized().Yaw;
	float DeltaYaw = ControlYawRotation - CoverRotation.Yaw;

	bool bIsFacingRight = EndVolume ? EndVolume->IsRightEnd() : DeltaYaw > 0.0f;

	this->LocomotionComponent->MaxWalkSpeed = this->LocomotionComponent->WalkSpeed;
	this->LocomotionComponent->bUseControllerDesiredRotation = false;
	this->LocomotionComponent->SetPlaneConstraintOrigin(CoverVolume->GetComponentLocation());
	this->LocomotionComponent->SetPlaneConstraintNormal(CoverVolume->GetForwardVector());
	this->LocomotionComponent->SetPlaneConstraintEnabled(true);
	this->LocomotionComponent->SnapUpdatedComponentToPlane();
	this->LocomotionComponent->EnterCoverEvent.Broadcast(bIsFacingRight);
}

void FCoverState::OnExit()
{
	FBaseCharacterState::OnExit();

	this->LocomotionComponent->bUseControllerDesiredRotation = true;
	this->LocomotionComponent->SetPlaneConstraintEnabled(false);
	this->LocomotionComponent->ExitCoverEvent.Broadcast();
}

void FCoverState::Update()
{
	if (!Character->IsInCover())
	{
		this->LocomotionComponent->ToPreviousState();
	}
}

FString FCoverState::ToString() const
{
	return FString(TEXT("Cover State"));
}
