// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CoverState.h"
#include "IdleState.h"
#include "WalkState.h"
#include "RunState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CharacterLocomotionComponent.generated.h"

DECLARE_EVENT_OneParam(UCharacterLocomotionComponent, FEnterCoverEvent, bool)
DECLARE_EVENT(UCharacterLocomotionComponent, FExitCoverEvent)

class FBaseCharacterState;

UCLASS()
class THIRDPERSONSHOOTER_API UCharacterLocomotionComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:

	friend class FBaseCharacterState;
	friend class FCoverState;
	friend class FIdleState;
	friend class FWalkState;
	friend class FRunState;

	FEnterCoverEvent EnterCoverEvent;
	FExitCoverEvent ExitCoverEvent;

	UCharacterLocomotionComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	float GetWalkSpeed() const;

	float GetRunSpeed() const;

	void ToCoverState();

	void ToIdleState();

	void ToWalkState();

	void ToRunState();

	void ToPreviousState();

protected:

	virtual void BeginPlay() override;

private:

	FBaseCharacterState* CurrentState;
	FBaseCharacterState* PreviousState;
	
	FCoverState CoverState;
	FIdleState IdleState;
	FWalkState WalkState;
	FRunState RunState;

	/** The speed of the character while walking. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)", meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float WalkSpeed;

	/** The speed of the character while running. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)", meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float RunSpeed;

	/** The speed of the character while in cover. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character Movement (General Settings)", meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float CoverSpeed;

	void ToState(FBaseCharacterState& State);
};
