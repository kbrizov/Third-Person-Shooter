// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawns/Enemies/AI/BaseAIController.h"
#include "EnemyAIController.generated.h"

class ABaseCharacter;

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API AEnemyAIController : public ABaseAIController
{
	GENERATED_BODY()

public:

	AEnemyAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "Target")
	AActor* GetTarget() const;

	UFUNCTION(BlueprintCallable, Category = "Target")
	FVector GetLastKnownTargetLocation() const;

	UFUNCTION(BlueprintCallable, Category = "Target")
	bool HasLostTarget() const;

	UFUNCTION(BlueprintCallable, Category = "Target")
	bool CanSeeTarget() const;

	UFUNCTION(BlueprintCallable, Category = "Character")
	void FireWeapon();

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

	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus) override;

private:

	AActor* Target;
	bool bHasLostTarget;
	FVector LastKnownTargetLocation;
	ABaseCharacter* ControlledPawn;
};
