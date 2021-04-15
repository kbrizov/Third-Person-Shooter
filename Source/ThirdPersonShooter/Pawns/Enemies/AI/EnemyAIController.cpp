// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonShooter/Pawns/Enemies/AI/EnemyAIController.h"
#include <Perception/AISenseConfig.h>
#include <Perception/AISense_Sight.h>
#include <Perception/AIPerceptionComponent.h>

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer),
	Target(nullptr),
	LastKnownTargetLocation(FVector::ZeroVector),
	ControlledPawn(nullptr)
{
	this->SetTeam(ETeam::Enemy);
}

bool AEnemyAIController::CanSeeTarget() const
{
	bool bCanSeeTarget = false;

	if (PerceptionComponent)
	{
		UAISenseConfig* SightConfig = this->PerceptionComponent->GetSenseConfig(UAISense::GetSenseID<UAISense_Sight>());

		if (SightConfig)
		{
			TArray<AActor*> OutCurrentlySeenActors = TArray<AActor*>();
			this->PerceptionComponent->GetCurrentlyPerceivedActors(SightConfig->GetSenseImplementation(), OutCurrentlySeenActors);

			if (OutCurrentlySeenActors.Num() > 0)
			{
				int32 OutIndex = 0;
				bCanSeeTarget = OutCurrentlySeenActors.Find(this->Target, OutIndex);
			}
		}
	}

	return bCanSeeTarget;
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	this->ControlledPawn = Cast<ABaseCharacter>(this->GetPawn());

	if (PerceptionComponent)
	{
		this->PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
	}
}

void AEnemyAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (PerceptionComponent)
	{
		this->PerceptionComponent->OnTargetPerceptionUpdated.RemoveDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
	}

	Super::EndPlay(EndPlayReason);
}

void AEnemyAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	if (Stimulus.WasSuccessfullySensed() && GetTeamAttitudeTowards(*Actor) == ETeamAttitude::Hostile)
	{
		this->Target = Actor;
		this->bHasLostTarget = false;
		this->LastKnownTargetLocation = Actor->GetActorLocation();
		this->SetFocus(this->Target);
	}
	else
	{
		this->Target = nullptr;
		this->bHasLostTarget = true;
		this->LastKnownTargetLocation = Actor->GetActorLocation();
		this->ClearFocus(EAIFocusPriority::Gameplay);
	}
}
