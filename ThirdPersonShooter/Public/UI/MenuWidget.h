// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BasePlayerWidget.h"
#include "MenuWidget.generated.h"

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API UMenuWidget : public UBasePlayerWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Player")
	bool IsPlayerDead() const;
};
