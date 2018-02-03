// Fill out your copyright notice in the Description page of Project Settings.

#include "MenuWidget.h"
#include "PlayerCharacter.h"

bool UMenuWidget::IsPlayerDead() const
{
	bool bIsPlayerDead = this->GetPlayerCharacter()->IsDead();

	return bIsPlayerDead;
}
