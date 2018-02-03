// Fill out your copyright notice in the Description page of Project Settings.

#include "HealthWidget.h"
#include "PlayerCharacter.h"

float UHealthWidget::GetHealthNormalized() const
{
	APlayerCharacter* PlayerCharacter = this->GetPlayerCharacter();
	float NormalizedHealth = 1.0f;

	if (PlayerCharacter)
	{
		NormalizedHealth = PlayerCharacter->GetCurrentHealthNormalized();
	}

	return NormalizedHealth;
}
