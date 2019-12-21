// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "BaseCharacter.h"

const FName ABaseAIController::AIPerceptionComponentName = FName(TEXT("AIPerception"));

ABaseAIController::ABaseAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->SetTeam(ETeam::Neutral);

	// Currently there is a bug https://issues.unrealengine.com/issue/UE-18378 which prevents DetectByAffiliation to be displayed in the editor. 
	// Adding the component through blueprints avoids this issue, so until the bug is fixed the component is added by blueprint.
	//this->PerceptionComponent = this->CreateDefaultSubobject<UAIPerceptionComponent>(ABaseAIController::AIPerceptionComponentName);
}

ETeam ABaseAIController::GetTeam() const
{
	return this->Team;
}

void ABaseAIController::SetTeam(ETeam NewTeam)
{
	this->Team = NewTeam;
}

void ABaseAIController::SetGenericTeamId(const FGenericTeamId& NewTeamId)
{
	Super::SetGenericTeamId(NewTeamId);
}

void ABaseAIController::SetGenericTeamId(ETeam NewTeam)
{
	uint8 Id = static_cast<uint8>(NewTeam);
	this->SetGenericTeamId(FGenericTeamId(Id));
}

ETeamAttitude::Type ABaseAIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	const ABaseCharacter* Character = Cast<const ABaseCharacter>(&Other);

	if (Character)
	{
		AController* Controller = Character->GetController();

		if (Controller)
		{
			return Super::GetTeamAttitudeTowards(*Controller);
		}
	}
	
	return Super::GetTeamAttitudeTowards(Other);
}

FVector ABaseAIController::GetFocalPointOnActor(const AActor* Actor) const
{
	FVector FocalPoint = FVector::ZeroVector;
	const ABaseCharacter* TargetActor = Cast<ABaseCharacter>(Actor);

	if (TargetActor)
	{
		FocalPoint = TargetActor->GetFocalPoint();
	}
	else
	{
		FocalPoint = Super::GetFocalPointOnActor(Actor);
	}

	return FocalPoint;
}

void ABaseAIController::BeginPlay()
{
	Super::BeginPlay();

	this->SetGenericTeamId(this->Team);
}

void ABaseAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// Empty by design. Unreal Engine doesn't support C++ pure virtual functions in UObjects, because every
	// such object needs to be able to be instanced (CDO - Class Default Object is created).
	// Check UE4 PURE_VIRTUAL macro for more info.
}
