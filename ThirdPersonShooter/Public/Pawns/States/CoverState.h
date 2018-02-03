// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacterState.h"

class THIRDPERSONSHOOTER_API FCoverState : public FBaseCharacterState
{
public:

	FCoverState();

	FCoverState(UCharacterLocomotionComponent* LocomotionComponent);

	virtual void OnEnter() override;

	virtual void OnExit() override;

	virtual void Update() override;

	virtual FString ToString() const override;
};
