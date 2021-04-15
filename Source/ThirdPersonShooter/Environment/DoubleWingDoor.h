// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include "DoubleWingDoor.generated.h"

class USoundBase;
class UBoxComponent;
class UAudioComponent;
class UStaticMeshComponent;

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API ADoubleWingDoor : public AActor
{
	GENERATED_BODY()
	
public:	

	static const FName AudioComponentName;
	static const FName FrameComponentName;
	static const FName LeftWingComponentName;
	static const FName RightWingComponentName;
	static const FName TriggerVolumeComponentName;

	ADoubleWingDoor(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaTime) override;

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	virtual void OnDoorTriggerBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnDoorTriggerEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	
	bool bIsOpening;
	bool bIsClosing;

	FVector LeftWingClosedLocation;
	FVector LeftWingOpenedLocation;

	FVector RightWingClosedLocation;
	FVector RightWingOpenedLocation;

	/** The value used for Linear Interpolation (Lerp) of the door's wings. Essentially the speed of the wings.*/
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "1", UIMin = "0", UIMax = "1"))
	float Alpha;

	/** Represents the distance a door wing travels when opening/closing. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float WingOffset;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door Audio", meta = (AllowPrivateAccess = "true"))
	USoundBase* OpeningSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Door Audio", meta = (AllowPrivateAccess = "true"))
	USoundBase* ClosingSound;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door Audio", meta = (AllowPrivateAccess = "true"))
	UAudioComponent* AudioComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* Frame;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* LeftWing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* RightWing;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Trigger Volume", meta = (AllowPrivateAccess = "true"))
	UBoxComponent* TriggerVolume;

	void SlideDoorWing(UStaticMeshComponent* DoorWing, const FVector& GoalLocation);

	bool IsOpen() const;

	bool IsClosed() const;
};
