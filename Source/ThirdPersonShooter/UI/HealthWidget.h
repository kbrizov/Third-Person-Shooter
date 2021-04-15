// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include "ThirdPersonShooter/UI/BasePlayerWidget.h"
#include "HealthWidget.generated.h"

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API UHealthWidget : public UBasePlayerWidget
{
	GENERATED_BODY()
	
public:

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthNormalized() const;
};
