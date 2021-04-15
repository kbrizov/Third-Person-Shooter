// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Character.h>
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

	virtual void AimStart();

	virtual void AimStop();

	virtual void FireWeaponStart();
		    
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

	void OnFireWeapon();

	void OnReloadWeapon();

protected:

	const FName RightHandSocketName = FName(TEXT("Right_Hand"));

	bool bIsWalking;
	bool bIsRunning;

	bool bIsAiming;
	bool bIsFiring;
	bool bIsReloading;

	bool bIsInCover;
	UCoverBoxComponent* OverlappingCoverVolume;
	UCoverEndComponent* OverlappingCoverEndVolume;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

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

inline void ABaseCharacter::AimStart()
{
	this->bIsAiming = true;
}

inline void ABaseCharacter::AimStop()
{
	this->bIsAiming = false;
}

inline float ABaseCharacter::GetMaxHealth() const
{
	return this->MaxHealth;
}

inline void ABaseCharacter::SetMaxHealth(float Value)
{
	this->MaxHealth = Value < 0.0f ? 0.0f : Value;
}

inline float ABaseCharacter::GetCurrentHealth() const
{
	return this->CurrentHealth;
}

inline void ABaseCharacter::SetCurrentHealth(float Value)
{
	this->CurrentHealth = FMath::Clamp(Value, 0.0f, MaxHealth);
}

inline float ABaseCharacter::GetCurrentHealthNormalized() const
{
	// NormalizedHealth is in [0, 1]
	// CurrentHealth is in [0, MaxHealth]
	// The ratio: NormalizedHealth / 1 = CurrentHealth / MaxHealth 
	float NormalizedHealth = this->CurrentHealth / this->MaxHealth;

	return NormalizedHealth;
}

inline UCoverBoxComponent* ABaseCharacter::GetOverlappingCoverVolume() const
{
	return this->OverlappingCoverVolume;
}

inline UCoverEndComponent* ABaseCharacter::GetOverlappingCoverEndVolume() const
{
	return this->OverlappingCoverEndVolume;
}

inline bool ABaseCharacter::IsIdle() const
{
	bool bIsMoving = this->GetVelocity().SizeSquared() > 0.0f;

	return !bIsMoving;
}

inline bool ABaseCharacter::IsWalking() const
{
	return this->bIsWalking;
}

inline bool ABaseCharacter::IsRunning() const
{
	return this->bIsRunning;
}

inline bool ABaseCharacter::IsFiring() const
{
	return this->bIsFiring;
}

inline void ABaseCharacter::SetIsFiring(bool bValue)
{
	this->bIsFiring = bValue;
}

inline bool ABaseCharacter::IsReloading() const
{
	return this->bIsReloading;
}

inline void ABaseCharacter::SetIsReloading(bool bValue)
{
	this->bIsReloading = bValue;
}

inline bool ABaseCharacter::IsInCover() const
{
	return this->bIsInCover;
}

inline bool ABaseCharacter::IsDead() const
{
	bool bIsAlive = this->CurrentHealth > 0.0f;

	return !bIsAlive;
}

inline void ABaseCharacter::OnFireWeapon()
{
	this->FireWeaponEvent.Broadcast();
}

inline void ABaseCharacter::OnReloadWeapon()
{
	this->ReloadWeaponEvent.Broadcast();
}
