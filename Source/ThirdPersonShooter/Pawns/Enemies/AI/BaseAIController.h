// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <AIController.h>
#include <Perception/AIPerceptionTypes.h>
#include "ThirdPersonShooter/Interfaces/TeamAgentInterface.h"
#include "BaseAIController.generated.h"

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API ABaseAIController : public AAIController, public ITeamAgentInterface
{
	GENERATED_BODY()
	
public:

	static const FName AIPerceptionComponentName;

	ABaseAIController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category = "Team")
	virtual ETeam GetTeam() const override; // ITeamAgentInterface

	UFUNCTION(BlueprintCallable, Category = "Team")
	virtual void SetTeam(ETeam NewTeam) override; // ITeamAgentInterface

	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamId) override;

	virtual void SetGenericTeamId(ETeam NewTeam);

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	virtual FVector GetFocalPointOnActor(const AActor* Actor) const override;

protected:

	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Team", meta = (AllowPrivateAccess = "true"))
	ETeam Team;
};
