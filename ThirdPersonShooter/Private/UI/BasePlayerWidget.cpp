// Fill out your copyright notice in the Description page of Project Settings.

#include "BasePlayerWidget.h"
#include "PlayerCharacter.h"

APlayerCharacter* UBasePlayerWidget::GetPlayerCharacter() const
{
	return this->PlayerCharacter;
}

void UBasePlayerWidget::NativeConstruct()
{
	this->PlayerCharacter = Cast<APlayerCharacter>(this->GetOwningPlayerPawn());
}
