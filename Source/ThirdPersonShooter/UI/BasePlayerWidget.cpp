// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonShooter/UI/BasePlayerWidget.h"
#include "ThirdPersonShooter/Pawns/Player/PlayerCharacter.h"

void UBasePlayerWidget::NativeConstruct()
{
	this->PlayerCharacter = Cast<APlayerCharacter>(this->GetOwningPlayerPawn());
}
