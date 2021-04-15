// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <Blueprint/UserWidget.h>
#include "BasePlayerWidget.generated.h"

class APlayerCharacter;

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API UBasePlayerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	APlayerCharacter* GetPlayerCharacter() const;

protected:

	virtual void NativeConstruct() override;

private:

	APlayerCharacter* PlayerCharacter;
};

inline APlayerCharacter* UBasePlayerWidget::GetPlayerCharacter() const
{
	return this->PlayerCharacter;
}
