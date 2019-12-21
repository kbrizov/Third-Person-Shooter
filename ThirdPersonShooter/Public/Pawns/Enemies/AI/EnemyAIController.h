// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "Pawns/Enemies/AI/BaseAIController.h"
#include "EnemyAIController.generated.h"

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API AEnemyAIController : public ABaseAIController
{
	GENERATED_BODY()

public:

	AEnemyAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Target")
	AActor* GetTarget() const;

	UFUNCTION(BlueprintCallable, Category = "Target")
	FVector GetLastKnownTargetLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Target")
	bool HasLostTarget() const;

	UFUNCTION(BlueprintCallable, Category = "Target")
	bool CanSeeTarget() const;

	UFUNCTION(BlueprintCallable, Category = "Character")
	void FireWeaponStart();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void FireWeaponStop();

	UFUNCTION(BlueprintCallable, Category = "Character")
	void ReloadWeapon();

	UFUNCTION(BlueprintCallable, Category = "Character")
	bool IsFiring() const;

	UFUNCTION(BlueprintCallable, Category = "Character")
	bool IsReloading() const;

	UFUNCTION(BlueprintCallable, Category = "Character")
	bool IsDead() const;

protected:

	virtual void BeginPlay() override;

	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;

private:

	AActor* Target;
	bool bHasLostTarget;
	FVector LastKnownTargetLocation;
	ABaseCharacter* ControlledPawn;
};

inline AActor* AEnemyAIController::GetTarget() const
{
	return this->Target;
}

inline FVector AEnemyAIController::GetLastKnownTargetLocation() const
{
	return this->LastKnownTargetLocation;
}

inline bool AEnemyAIController::HasLostTarget() const
{
	return this->bHasLostTarget;
}

inline void AEnemyAIController::FireWeaponStart()
{
	if (ControlledPawn)
	{
		this->ControlledPawn->FireWeaponStart();
	}
}

inline void AEnemyAIController::FireWeaponStop()
{
	if (ControlledPawn)
	{
		this->ControlledPawn->FireWeaponStop();
	}
}

inline void AEnemyAIController::ReloadWeapon()
{
	if (ControlledPawn)
	{
		this->ControlledPawn->ReloadWeapon();
	}
}

inline bool AEnemyAIController::IsFiring() const
{
	bool bIsFiring = false;

	if (ControlledPawn)
	{
		bIsFiring = this->ControlledPawn->IsFiring();
	}

	return bIsFiring;
}

inline bool AEnemyAIController::IsReloading() const
{
	bool bIsReloading = false;

	if (ControlledPawn)
	{
		bIsReloading = this->ControlledPawn->IsReloading();
	}

	return bIsReloading;
}

inline bool AEnemyAIController::IsDead() const
{
	bool bIsDead = false;

	if (ControlledPawn)
	{
		bIsDead = this->ControlledPawn->IsDead();
	}

	return bIsDead;
}
