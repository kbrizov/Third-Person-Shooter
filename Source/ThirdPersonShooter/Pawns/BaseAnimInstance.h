// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Animation/AnimInstance.h>
#include "BaseAnimInstance.generated.h"

class ABaseCharacter;

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API UBaseAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	void NativeInitializeAnimation() override;

	void NativeUpdateAnimation(float DeltaTimeX) override;

protected:

	UFUNCTION(BlueprintCallable, Category = "Character")
	void SetIsReloading(bool bValue);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "FireWeapon"))
	void OnFireWeapon();

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ReloadWeapon"))
	void OnReloadWeapon();

	UFUNCTION()
	void OnEnterCover(bool bIsInCoverRight);

	UFUNCTION()
	void OnExitCover();

private:

	FRotator PreviousActorRotation = FRotator::ZeroRotator;

	UPROPERTY(BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	ABaseCharacter* Character = nullptr;

	/** A value used for movement blending. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement Related", meta = (AllowPrivateAccess = "true", ClampMin = "-1", ClampMax = "1", UIMin = "-1", UIMax = "1"))
	float MoveRight = 0.0f;

	/** A value used for movement blending. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Movement Related", meta = (AllowPrivateAccess = "true", ClampMin = "-1", ClampMax = "1", UIMin = "-1", UIMax = "1"))
	float MoveForward = 0.0f;

	/** A value used for Aim Offset blending. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim Offset Related", meta = (AllowPrivateAccess = "true", ClampMin = "-90", ClampMax = "90", UIMin = "-90", UIMax = "90"))
	float LookYawAngle = 0.0f;

	/** A value used for Aim Offset blending. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim Offset Related", meta = (AllowPrivateAccess = "true", ClampMin = "-90", ClampMax = "90", UIMin = "-90", UIMax = "90"))
	float LookPitchAngle = 0.0f;

	/** The rate with which the Aim Offset angles change during interpolation. Set to 0 or -1 if instant interpolation is required. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Aim Offset Related", meta = (AllowPrivateAccess = "true", ClampMin = "-1", UIMin = "-1"))
	float AimOffsetInterpolationSpeed = 0.0f;

	/** Indicates if the pawn is rotating LEFT in place. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Related", meta = (AllowPrivateAccess = "true"))
	bool bIsTurningLeft = false;

	/** Indicates if the pawn is rotating RIGHT in place. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Turn Related", meta = (AllowPrivateAccess = "true"))
	bool bIsTurningRight = false;

	/** Indicates if the pawn is currently RELOADING a weapon. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon Related", meta = (AllowPrivateAccess = "true"))
	bool bIsReloadingWeapon = false;

	/** Indicates if the pawn is in cover. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cover Related", meta = (AllowPrivateAccess = "true"))
	bool bIsInCover = false;

	/** Indicates if the pawn facing right when in cover. If false, the pawn is facing left. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Cover Related", meta = (AllowPrivateAccess = "true"))
	bool bIsFacingRight = false;

	void UpdateMovementVariables();

	void UpdateLookAngles(float DeltaTime);

	void UpdateTurnVariables();
};
