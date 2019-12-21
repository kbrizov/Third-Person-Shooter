// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawns/BaseCharacter.h"
#include "PlayerCharacter.generated.h"

class UCameraComponent;
class UElasticArmComponent;

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()
	
public:

	static const FName ElasticArmComponentName;
	static const FName CameraComponentName;

	APlayerCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual void FireWeaponStart() override;

	virtual void FireWeaponStop() override;

protected:

	const FName MoveRightAxisName = FName(TEXT("MoveRight"));
	const FName MoveForwardAxisName = FName(TEXT("MoveForward"));
	const FName TurnAxisName = FName(TEXT("Turn"));
	const FName LookUpAxisName = FName(TEXT("LookUp"));

	const FName AimActionName = FName(TEXT("Aim"));
	const FName FireWeaponActionName = FName(TEXT("FireWeapon"));
	const FName ReloadWeaponActionName = FName(TEXT("ReloadWeapon"));
	const FName ToggleWalkRunActionName = FName(TEXT("ToggleWalkRun"));
	const FName ToggleCoverActionName = FName(TEXT("ToggleCover"));

private:

	bool bIsRecoilCompensationEnabled;
	FRotator InitialControlRotation;
	FRotator TargetControlRotation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UElasticArmComponent* ElasticArm;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	UCameraComponent* Camera;

	void OnMoveRight(float AxisValue);

	void OnMoveForward(float AxisValue);

	void OnTurn(float AxisValue);

	void OnLookUp(float AxisValue);

	void OnAimStart();

	void OnAimStop();

	void OnFireWeaponStart();

	void OnFireWeaponStop();

	void OnReloadWeapon();

	void OnToggleWalkRun();

	void OnToggleCover();

	void CompensateRecoil(float DeltaTime);
};
