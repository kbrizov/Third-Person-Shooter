// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "TeamAgentInterface.generated.h"

UENUM(BlueprintType)
enum class ETeam : uint8
{
	Player = 0,
	Enemy = 127,
	Neutral = 255
};

// This class does not need to be modified. It's only for the build tool.
UINTERFACE(MinimalAPI)
class UTeamAgentInterface : public UInterface
{
	GENERATED_BODY()
};

class THIRDPERSONSHOOTER_API ITeamAgentInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	
	virtual ETeam GetTeam() const = 0;

	virtual void SetTeam(ETeam NewTeam) = 0;
};
