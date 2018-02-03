// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

DECLARE_EVENT(ABaseCharacter, FDeathEvent)
DECLARE_EVENT(ABaseCharacter, FFireWeaponEvent)
DECLARE_EVENT(ABaseCharacter, FReloadWeaponEvent)

class ABaseWeapon;
class UCoverBoxComponent;
class UCoverEndComponent;

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	static const FName FocalPointComponentName;
	static const FName WeaponComponentName;

	FDeathEvent DeathEvent;
	FFireWeaponEvent FireWeaponEvent;
	FReloadWeaponEvent ReloadWeaponEvent;

	ABaseCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual void Aim();

	virtual void AimStop();

	virtual void FireWeapon();
		    
	virtual void FireWeaponStop();

	virtual void ReloadWeapon();

	virtual FVector GetFocalPoint() const;

	virtual void GetViewPoint(FVector& OutLocation, FRotator& OutRotation) const;

	ABaseWeapon* GetEquipedWeapon() const;

	void EquipWeapon(ABaseWeapon* Weapon);

	float GetMaxHealth() const;

	float GetCurrentHealth() const;

	float GetCurrentHealthNormalized() const;

	UCoverBoxComponent* GetOverlappingCoverVolume() const;

	UCoverEndComponent* GetOverlappingCoverEndVolume() const;

	bool IsIdle() const;

	bool IsWalking() const;

	bool IsRunning() const;

	bool IsFiring() const;

	void SetIsFiring(bool bValue);

	bool IsReloading() const;

	void SetIsReloading(bool bValue);

	bool IsInCover() const;

	bool IsDead() const;

protected:

	bool bIsWalking;
	bool bIsRunning;

	bool bIsAiming;
	bool bIsFiring;
	bool bIsReloading;

	bool bIsInCover;
	UCoverBoxComponent* OverlappingCoverVolume;
	UCoverEndComponent* OverlappingCoverEndVolume;

	const FName RightHandSocketName = FName(TEXT("Right_Hand"));

	virtual void BeginPlay() override;

	virtual void Die();

	bool IsFacingCover() const;

	bool CanTakeCover() const;

	bool CanExitCover() const;

	bool CanFireWeapon() const;

	bool CanReloadWeapon() const;

	void SetMaxHealth(float Value);

	void SetCurrentHealth(float Value);

	UFUNCTION()
	virtual void OnCoverVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnCoverVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

	UFUNCTION()
	virtual void OnCoverEndVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	virtual void OnCoverEndVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex);

private:

	float CurrentHealth;

	/** The max amount of damage the character can take before dying. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float MaxHealth;

	/** A scene component at which NPCs can aim. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", meta = (AllowPrivateAccess = "true"))
	USceneComponent* FocalPoint;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	UChildActorComponent* Weapon;
};
