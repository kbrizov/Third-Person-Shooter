// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class ABaseCharacter;
class UCharacterLocomotionComponent;

class THIRDPERSONSHOOTER_API FBaseCharacterState
{
public:

	virtual ~FBaseCharacterState() = 0;

	virtual void OnEnter();

	virtual void OnExit();

	virtual void Update();

	virtual FString ToString() const = 0;

protected:

	ABaseCharacter* Character;
	UCharacterLocomotionComponent* LocomotionComponent;

	FBaseCharacterState(UCharacterLocomotionComponent* LocomotionComponent);
};
