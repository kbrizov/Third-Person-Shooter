// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonShooter/Pawns/Player/PlayerCharacter.h"
#include "ThirdPersonShooter/Weapons/BaseWeapon.h"
#include "ThirdPersonShooter/Miscellaneous/CoverBoxComponent.h"
#include "ThirdPersonShooter/Miscellaneous/CoverEndComponent.h"
#include "ThirdPersonShooter/Miscellaneous/ElasticArmComponent.h"
#include <Camera/CameraComponent.h>
#include <Components/InputComponent.h>
#include <GameFramework/Controller.h>

const FName APlayerCharacter::ElasticArmComponentName = FName(TEXT("ElasticArm"));
const FName APlayerCharacter::CameraComponentName = FName(TEXT("Camera"));

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer),
	bIsRecoilCompensationEnabled(false),
	InitialControlRotation(FRotator::ZeroRotator),
	TargetControlRotation(FRotator::ZeroRotator)
{
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

	PlayerInputComponent->BindAction(this->AimActionName, EInputEvent::IE_Pressed, this, &APlayerCharacter::OnAimStart);
	PlayerInputComponent->BindAction(this->AimActionName, EInputEvent::IE_Released, this, &APlayerCharacter::OnAimStop);
	PlayerInputComponent->BindAction(this->FireWeaponActionName, EInputEvent::IE_Pressed, this, &APlayerCharacter::OnFireWeaponStart);
	PlayerInputComponent->BindAction(this->FireWeaponActionName, EInputEvent::IE_Released, this, &APlayerCharacter::OnFireWeaponStop);
	PlayerInputComponent->BindAction(this->ReloadWeaponActionName, EInputEvent::IE_Pressed, this, &APlayerCharacter::OnReloadWeapon);
	PlayerInputComponent->BindAction(this->ToggleWalkRunActionName, EInputEvent::IE_Pressed, this, &APlayerCharacter::OnToggleWalkRun);
	PlayerInputComponent->BindAction(this->ToggleCoverActionName, EInputEvent::IE_Pressed, this, &APlayerCharacter::OnToggleCover);
}

void APlayerCharacter::FireWeaponStart()
{
	if (!CanFireWeapon())
	{
		return;
	}

	this->InitialControlRotation = this->GetControlRotation().GetNormalized();

	Super::FireWeaponStart();
}

void APlayerCharacter::FireWeaponStop()
{
	if (!IsFiring())
	{
		return;
	}

	Super::FireWeaponStop();

	FRotator AccumulatedRecoil = this->GetEquipedWeapon()->GetAccumulatedRecoil();
	const FRotator CurrentControlRotation = this->GetControlRotation().GetNormalized();
	const FRotator DeltaRotation = CurrentControlRotation - this->InitialControlRotation;
	const FRotator InputRotation = DeltaRotation - AccumulatedRecoil;

	if (InputRotation.Pitch < 0.0f) // Current < Initial (Rotation opposite to the recoil, negating it).
	{
		const float DeltaPitch = AccumulatedRecoil.Pitch - FMath::Abs(InputRotation.Pitch); // AccumulatedRecoil.Pitch is always positive.
		AccumulatedRecoil.Pitch = (DeltaPitch < 0.0f) ? 0.0f : DeltaPitch;
	}

	this->TargetControlRotation = CurrentControlRotation - AccumulatedRecoil;
	this->bIsRecoilCompensationEnabled = true;
}

void APlayerCharacter::CompensateRecoil(float DeltaTime)
{
	const FRotator CurrentControlRotation = this->GetControlRotation();
	const FRotator NewControlRotation = FMath::RInterpTo(CurrentControlRotation, this->TargetControlRotation, DeltaTime, 16.0f);

	AController* Controller = this->GetController();
	check(Controller);

	Controller->SetControlRotation(NewControlRotation);

	if (TargetControlRotation.Equals(NewControlRotation, 0.1f) || IsFiring())
	{
		this->bIsRecoilCompensationEnabled = false;
	}

	const float Threshold = 0.01f;
	const float PitchInput = FMath::Abs(this->GetInputAxisValue(this->LookUpAxisName));
	const float YawInput = FMath::Abs(this->GetInputAxisValue(this->TurnAxisName));
	
	if (PitchInput > Threshold)
	{
		this->bIsRecoilCompensationEnabled = false;
	}

	if (YawInput > Threshold)
	{
		const FRotator CurrentControlRotation = this->GetControlRotation();
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
			const bool bIsRightEnd = this->OverlappingCoverEndVolume->IsRightEnd();

			const bool bShouldMoveLeft = AxisValue < 0.0f;
			const bool bShouldMoveRight = AxisValue > 0.0f;

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

	const FRotationMatrix RotationMatrix = FRotationMatrix(YawRotation);
	const FVector Direction = RotationMatrix.GetUnitAxis(EAxis::Y);

	this->AddMovementInput(Direction, AxisValue);
}

void APlayerCharacter::OnMoveForward(float AxisValue)
{
	if (FMath::IsNearlyZero(AxisValue) || IsInCover())
	{
		return;
	}

	const FRotator ControlYawRotation = FRotator(0.0f, GetControlRotation().Yaw, 0.0f);
	const FRotationMatrix RotationMatrix = FRotationMatrix(ControlYawRotation);
	const FVector Direction = RotationMatrix.GetUnitAxis(EAxis::X);

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

void APlayerCharacter::OnAimStart()
{
	this->ElasticArm->UseMinArmLength();
	this->AimStart();
}

void APlayerCharacter::OnAimStop()
{
	this->ElasticArm->UseMaxArmLength();
	this->AimStop();
}

void APlayerCharacter::OnFireWeaponStart()
{
	this->FireWeaponStart();
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
