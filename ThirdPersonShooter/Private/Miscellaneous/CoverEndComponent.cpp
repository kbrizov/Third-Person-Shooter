// Fill out your copyright notice in the Description page of Project Settings.

#include "CoverEndComponent.h"

bool UCoverEndComponent::IsRightEnd() const
{
	bool bIsRightEnd = (this->Type == ECoverEndType::Right);

	return bIsRightEnd;
}

bool UCoverEndComponent::IsLeftEnd() const
{
	bool bIsLeftEnd = !IsRightEnd();

	return bIsLeftEnd;
}
