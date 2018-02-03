// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "CoverEndComponent.generated.h"

UENUM(BlueprintType)
enum class ECoverEndType : uint8
{
	Left = 0,
	Right = 2
};

UCLASS(meta = (BlueprintSpawnableComponent))
class THIRDPERSONSHOOTER_API UCoverEndComponent : public UBoxComponent
{
	GENERATED_BODY()
	
public:

	bool IsRightEnd() const;

	bool IsLeftEnd() const;

private:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Type", meta = (AllowPrivateAccess = "true"))
	ECoverEndType Type = ECoverEndType::Left;
};
