// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include "BaseProjectile.generated.h"

class ABaseWeapon;
class UParticleSystem;
class USphereComponent;
class UParticleSystemComponent;
class UProjectileMovementComponent;

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API ABaseProjectile : public AActor
{
	GENERATED_BODY()
	
public:	

	static const FName CollisionComponentName;
	static const FName ParticleSystemComponentName;
	static const FName ProjectileMovementComponentName;

	ABaseProjectile(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	ABaseWeapon* GetWeapon() const;

	void SetWeapon(ABaseWeapon* Weapon);

	UParticleSystem* GetMuzzleFlashParticle() const;

protected:

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult);

private:

	/** An pointer to the weapon that fired the projectile. */
	ABaseWeapon* Weapon;

	/** The amount of damage the projectile does. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float Damage;

	/** The impulse force to apply when the projectile hits something. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Projectile", meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float ImpulseForce;

	/** The particle effect at the muzzle of the weapon which fires the projectile. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* MuzzleFlashParticle;

	/** The particle effect to be spawned on impact (OnHit). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ImpactParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	USphereComponent* CollisionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Projectile", meta = (AllowPrivateAccess = "true"))
	UParticleSystemComponent* ParticleSystem;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;
};

inline ABaseWeapon* ABaseProjectile::GetWeapon() const
{
	return this->Weapon;
}

inline void ABaseProjectile::SetWeapon(ABaseWeapon* Weapon)
{
	this->Weapon = Weapon;
}

inline UParticleSystem* ABaseProjectile::GetMuzzleFlashParticle() const
{
	return this->MuzzleFlashParticle;
}
