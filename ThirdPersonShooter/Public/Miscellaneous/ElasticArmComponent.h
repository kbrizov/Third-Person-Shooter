// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SpringArmComponent.h"
#include "ElasticArmComponent.generated.h"

UCLASS(meta = (BlueprintSpawnableComponent))
class THIRDPERSONSHOOTER_API UElasticArmComponent : public USpringArmComponent
{
	GENERATED_BODY()

public:

	UElasticArmComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void UseMinArmLength();

	void UseMaxArmLength();

protected:

	virtual void UpdateDesiredArmLocation(bool bDoTrace, bool bDoLocationLag, bool bDoRotationLag, float DeltaTime) override;

private:

	bool bShouldUseMinArmLength;
	bool bShouldUseMaxArmLength;

	/** The minimum length of the arm. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float MinArmLength;

	/** The maximum length of the arm. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float MaxArmLength;

	/** The speed of the interpolation when the length is changed. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Camera, meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float ArmLengthChangeSpeed;
};

inline void UElasticArmComponent::UseMinArmLength()
{
	this->bShouldUseMinArmLength = true;
	this->bShouldUseMaxArmLength = false;
}

inline void UElasticArmComponent::UseMaxArmLength()
{
	this->bShouldUseMinArmLength = false;
	this->bShouldUseMaxArmLength = true;
}
