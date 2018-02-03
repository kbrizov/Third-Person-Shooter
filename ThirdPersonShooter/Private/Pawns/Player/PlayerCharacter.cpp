// Fill out your copyright notice in the Description page of Project Settings.

#include "PlayerCharacter.h"
#include "BaseWeapon.h"
#include "CoverBoxComponent.h"
#include "CoverEndComponent.h"
#include "ElasticArmComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"

const FName APlayerCharacter::ElasticArmComponentName = FName(TEXT("ElasticArm"));
const FName APlayerCharacter::CameraComponentName = FName(TEXT("Camera"));

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->bIsRecoilCompensationEnabled = false;
	this->InitialControlRotation = FRotator::ZeroRotator;
	this->TargetControlRotation = FRotator::ZeroRotator;

	this->ElasticArm = this->CreateDefaultSubobject<UElasticArmComponent>(APlayerCharacter::ElasticArmComponentName);
	this->ElasticArm->SetupAttachment(this->RootComponent);
	this->ElasticArm->bUsePawnControlRotation = true;

	this->Camera = this->CreateDefaultSubobject<UCameraComponent>(APlayerCharacter::CameraComponentName);
	this->Camera->SetupAttachment(this->ElasticArm, UElasticArmComponent::SocketName);
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsRecoilCompensationEnabled)
	{
		this->CompensateRecoil(DeltaTime);
	}
}

void APlayerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(this->MoveRightAxisName, this, &APlayerCharacter::OnMoveRight);
	PlayerInputComponent->BindAxis(this->MoveForwardAxisName, this, &APlayerCharacter::OnMoveForward);
	PlayerInputComponent->BindAxis(this->TurnAxisName, this, &APlayerCharacter::OnTurn);
	PlayerInputComponent->BindAxis(this->LookUpAxisName, this, &APlayerCharacter::OnLookUp);

	PlayerInputComponent->BindAction(this->AimActionName, EInputEvent::IE_Pressed, this, &APlayerCharacter::OnAim);
	PlayerInputComponent->BindAction(this->AimActionName, EInputEvent::IE_Released, this, &APlayerCharacter::OnAimStop);
	PlayerInputComponent->BindAction(this->FireWeaponActionName, EInputEvent::IE_Pressed, this, &APlayerCharacter::OnFireWeapon);
	PlayerInputComponent->BindAction(this->FireWeaponActionName, EInputEvent::IE_Released, this, &APlayerCharacter::OnFireWeaponStop);
	PlayerInputComponent->BindAction(this->ReloadWeaponActionName, EInputEvent::IE_Pressed, this, &APlayerCharacter::OnReloadWeapon);
	PlayerInputComponent->BindAction(this->ToggleWalkRunActionName, EInputEvent::IE_Pressed, this, &APlayerCharacter::OnToggleWalkRun);
	PlayerInputComponent->BindAction(this->ToggleCoverActionName, EInputEvent::IE_Pressed, this, &APlayerCharacter::OnToggleCover);
}

void APlayerCharacter::FireWeapon()
{
	if (!CanFireWeapon())
	{
		return;
	}

	this->InitialControlRotation = this->GetControlRotation().GetNormalized();

	Super::FireWeapon();
}

void APlayerCharacter::FireWeaponStop()
{
	if (!IsFiring())
	{
		return;
	}

	Super::FireWeaponStop();

	FRotator CurrentControlRotation = this->GetControlRotation().GetNormalized();
	FRotator DeltaRotation = CurrentControlRotation - this->InitialControlRotation;
	FRotator AccumulatedRecoil = this->GetEquipedWeapon()->GetAccumulatedRecoil();
	FRotator InputRotation = DeltaRotation - AccumulatedRecoil;

	if (InputRotation.Pitch < 0.0f) // Current < Initial (Rotation opposite to the recoil, negating it).
	{
		float DeltaPitch = AccumulatedRecoil.Pitch - FMath::Abs(InputRotation.Pitch); // AccumulatedRecoil.Pitch is always positive.
		AccumulatedRecoil.Pitch = (DeltaPitch < 0.0f) ? 0.0f : DeltaPitch;
	}

	this->TargetControlRotation = CurrentControlRotation - AccumulatedRecoil;
	this->bIsRecoilCompensationEnabled = true;
}

void APlayerCharacter::CompensateRecoil(float DeltaTime)
{
	FRotator CurrentControlRotation = this->GetControlRotation();
	FRotator NewControlRotation = FMath::RInterpTo(CurrentControlRotation, this->TargetControlRotation, DeltaTime, 16.0f);
	this->GetController()->SetControlRotation(NewControlRotation);

	if (TargetControlRotation.Equals(NewControlRotation, 0.1f) || IsFiring())
	{
		this->bIsRecoilCompensationEnabled = false;
	}

	float Threshold = 0.01f;
	float PitchInput = FMath::Abs(this->GetInputAxisValue(this->LookUpAxisName));
	float YawInput = FMath::Abs(this->GetInputAxisValue(this->TurnAxisName));
	
	if (PitchInput > Threshold)
	{
		this->bIsRecoilCompensationEnabled = false;
	}

	if (YawInput > Threshold)
	{
		FRotator CurrentControlRotation = this->GetControlRotation();
		this->TargetControlRotation.Yaw = CurrentControlRotation.Yaw;
	}
}

void APlayerCharacter::OnMoveRight(float AxisValue)
{
	if (FMath::IsNearlyZero(AxisValue))
	{
		return;
	}

	FRotator YawRotation = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);

	if (IsInCover())
	{
		bool bIsFacingCover = this->IsFacingCover();
		
		if (OverlappingCoverEndVolume)
		{
			bool bIsRightEnd = this->OverlappingCoverEndVolume->IsRightEnd();

			bool bShouldMoveLeft = AxisValue < 0.0f;
			bool bShouldMoveRight = AxisValue > 0.0f;

			bool bCanMoveLeft = false;
			bool bCanMoveRight = false;

			if (bIsRightEnd)
			{
				bCanMoveLeft = bIsFacingCover ? true : false;
				bCanMoveRight = bIsFacingCover ? false : true;
			}
			else // Is left end.
			{
				bCanMoveLeft = bIsFacingCover ? false : true;
				bCanMoveRight = bIsFacingCover ? true : false;
			}

			if ((bShouldMoveLeft && !bCanMoveLeft) || (bShouldMoveRight && !bCanMoveRight))
			{
				return;
			}
		}

		AxisValue = bIsFacingCover ? AxisValue : -AxisValue;

		YawRotation = FRotator(0.0f, OverlappingCoverVolume->GetComponentRotation().Yaw, 0.0f);
	}

	FRotationMatrix RotationMatrix = FRotationMatrix(YawRotation);
	FVector Direction = RotationMatrix.GetUnitAxis(EAxis::Y);

	this->AddMovementInput(Direction, AxisValue);
}

void APlayerCharacter::OnMoveForward(float AxisValue)
{
	if (FMath::IsNearlyZero(AxisValue) || IsInCover())
	{
		return;
	}

	FRotator ControlYawRotation = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);
	FRotationMatrix RotationMatrix = FRotationMatrix(ControlYawRotation);
	FVector Direction = RotationMatrix.GetUnitAxis(EAxis::X);

	this->AddMovementInput(Direction, AxisValue);
}

void APlayerCharacter::OnTurn(float AxisValue)
{
	this->AddControllerYawInput(AxisValue);
}

void APlayerCharacter::OnLookUp(float AxisValue)
{
	this->AddControllerPitchInput(AxisValue);
}

void APlayerCharacter::OnAim()
{
	this->ElasticArm->UseMinArmLength();
	this->Aim();
}

void APlayerCharacter::OnAimStop()
{
	this->ElasticArm->UseMaxArmLength();
	this->AimStop();
}

void APlayerCharacter::OnFireWeapon()
{
	this->FireWeapon();
}

void APlayerCharacter::OnFireWeaponStop()
{
	this->FireWeaponStop();
}

void APlayerCharacter::OnReloadWeapon()
{
	this->ReloadWeapon();
}

void APlayerCharacter::OnToggleWalkRun()
{
	this->bIsWalking = !bIsWalking;
	this->bIsRunning = !bIsRunning;
}

void APlayerCharacter::OnToggleCover()
{
	if (!bIsInCover && CanTakeCover())
	{
		this->bIsInCover = true;
	}
	else if (bIsInCover && CanExitCover())
	{
		this->bIsInCover = false;
	}
}
