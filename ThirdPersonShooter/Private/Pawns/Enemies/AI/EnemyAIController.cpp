// Fill out your copyright notice in the Description page of Project Settings.

#include "EnemyAIController.h"
#include "BaseCharacter.h"
#include "Perception/AISenseConfig.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AIPerceptionComponent.h"

AEnemyAIController::AEnemyAIController(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->Target = nullptr;
	this->LastKnownTargetLocation = FVector::ZeroVector;
	this->ControlledPawn = nullptr; 
	this->SetTeam(ETeam::Enemy);
}

void AEnemyAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AActor* AEnemyAIController::GetTarget() const
{
	return this->Target;
}

FVector AEnemyAIController::GetLastKnownTargetLocation() const
{
	return this->LastKnownTargetLocation;
}

bool AEnemyAIController::HasLostTarget() const
{
	return this->bHasLostTarget;
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

void AEnemyAIController::FireWeapon()
{
	if (ControlledPawn)
	{
		this->ControlledPawn->FireWeapon();
	}
}

void AEnemyAIController::FireWeaponStop()
{
	if (ControlledPawn)
	{
		this->ControlledPawn->FireWeaponStop();
	}
}

void AEnemyAIController::ReloadWeapon()
{
	if (ControlledPawn)
	{
		this->ControlledPawn->ReloadWeapon();
	}
}

bool AEnemyAIController::IsFiring() const
{
	bool bIsFiring = false;

	if (ControlledPawn)
	{
		bIsFiring = this->ControlledPawn->IsFiring();
	}

	return bIsFiring;
}

bool AEnemyAIController::IsReloading() const
{
	bool bIsReloading = false;

	if (ControlledPawn)
	{
		bIsReloading = this->ControlledPawn->IsReloading();
	}

	return bIsReloading;
}

bool AEnemyAIController::IsDead() const
{
	bool bIsDead = false;

	if (ControlledPawn)
	{
		bIsDead = this->ControlledPawn->IsDead();
	}

	return bIsDead;
}

void AEnemyAIController::BeginPlay()
{
	Super::BeginPlay();

	this->ControlledPawn = Cast<ABaseCharacter>(this->GetPawn());

	if (PerceptionComponent)
	{
		PerceptionComponent->OnTargetPerceptionUpdated.AddDynamic(this, &AEnemyAIController::OnTargetPerceptionUpdated);
	}
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
