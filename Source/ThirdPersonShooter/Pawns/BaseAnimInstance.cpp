// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonShooter/Pawns/BaseAnimInstance.h"
#include "ThirdPersonShooter/Pawns/BaseCharacter.h"
#include "ThirdPersonShooter/Pawns/CharacterLocomotionComponent.h"

void UBaseAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	this->Character = Cast<ABaseCharacter>(this->TryGetPawnOwner());

	if (Character)
	{
		this->Character->FireWeaponEvent.AddUFunction(this, "OnFireWeapon");
		this->Character->ReloadWeaponEvent.AddUFunction(this, "OnReloadWeapon");

		UCharacterLocomotionComponent* LocomotionComponent = Cast<UCharacterLocomotionComponent>(this->Character->GetCharacterMovement());
		check(LocomotionComponent);

		LocomotionComponent->EnterCoverEvent.AddUFunction(this, "OnEnterCover");
		LocomotionComponent->ExitCoverEvent.AddUFunction(this, "OnExitCover");
	}
}

void UBaseAnimInstance::NativeUpdateAnimation(float DeltaTimeX)
{
	Super::NativeUpdateAnimation(DeltaTimeX);

	// TODO: Implement event based variable updating where applicable.
	if (Character)
	{
		this->UpdateMovementVariables();
		this->UpdateLookAngles(DeltaTimeX);
		this->UpdateTurnVariables();
	}
}

void UBaseAnimInstance::SetIsReloading(bool bValue)
{
	this->Character->SetIsReloading(bValue);
	this->bIsReloadingWeapon = bValue;
}

void UBaseAnimInstance::OnEnterCover(bool bIsFacingRight)
{
	this->bIsInCover = true;
	this->bIsFacingRight = bIsFacingRight;
}

void UBaseAnimInstance::OnExitCover()
{
	this->bIsInCover = false;
}

void UBaseAnimInstance::UpdateMovementVariables()
{
	const FVector WorldSpaceVelocity = this->Character->GetVelocity();
	const FVector LocalSpaceVelocity = this->Character->GetActorTransform().InverseTransformVector(WorldSpaceVelocity);
	const float MaxSpeed = Cast<UCharacterLocomotionComponent>(this->Character->GetCharacterMovement())->GetRunSpeed();

	const FVector2D InputRange = FVector2D(-MaxSpeed, MaxSpeed);
	const FVector2D OutputRange = FVector2D(-1.0f, 1.0f);

	this->MoveForward = FMath::GetMappedRangeValueClamped(InputRange, OutputRange, LocalSpaceVelocity.X);
	this->MoveRight = FMath::GetMappedRangeValueClamped(InputRange, OutputRange, LocalSpaceVelocity.Y);
}

void UBaseAnimInstance::UpdateLookAngles(float DeltaTime)
{
	const FRotator ControlRotation = this->Character->GetControlRotation();
	const FRotator ActorRotation = this->Character->GetActorRotation();
	const FRotator DeltaRotation = (ControlRotation - ActorRotation).GetNormalized();

	FRotator CurrentRotation = FRotator(LookPitchAngle, LookYawAngle, 0.0f);
	CurrentRotation = FMath::RInterpTo(CurrentRotation, DeltaRotation, DeltaTime, AimOffsetInterpolationSpeed);

	this->LookPitchAngle = FMath::Clamp(CurrentRotation.Pitch, -90.0f, 90.0f);
	this->LookYawAngle = FMath::Clamp(CurrentRotation.Yaw, -90.0f, 90.0f);
}

void UBaseAnimInstance::UpdateTurnVariables()
{
	const FRotator ActorRotation = this->Character->GetActorRotation();
	const FRotator DeltaRotation = (ActorRotation - PreviousActorRotation).GetNormalized();

	PreviousActorRotation = ActorRotation;
	const bool bIsIdle = Character->IsIdle();

	// TODO: Maybe event based so it's not changed every frame?
	if (DeltaRotation.Yaw > 0.0f && bIsIdle)
	{
		this->bIsTurningRight = true;
		this->bIsTurningLeft = false;
	}
	else if (DeltaRotation.Yaw < 0.0f && bIsIdle)
	{
		this->bIsTurningLeft = true;
		this->bIsTurningRight = false;
	}
	else
	{
		this->bIsTurningLeft = false;
		this->bIsTurningRight = false;
	}
}
