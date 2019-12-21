// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon.h"
#include "BaseCharacter.h"
#include "BaseProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

const FName ABaseWeapon::WeaponTraceTag = FName(TEXT("WeaponTrace"));
const FName ABaseWeapon::MeshComponentName = FName(TEXT("Mesh"));

ABaseWeapon::ABaseWeapon(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer),
	AccumulatedRecoil(FRotator::ZeroRotator)
{
	this->SetUser(nullptr); // The weapon is not equiped.
	this->SetRange(50.0f); // Range of 50m (5000cm).
	this->SetRecoil(1.0f);
	this->SetSpread(0.0f); // No spread.
	this->SetMagazineCapacity(12);
	this->SetRemainingAmmunition(this->Magazine.Capacity);

	this->Mesh = this->CreateDefaultSubobject<USkeletalMeshComponent>(ABaseWeapon::MeshComponentName);
	this->RootComponent = this->Mesh;
}

void ABaseWeapon::StartFire()
{
	this->AccumulatedRecoil = FRotator::ZeroRotator;
}

void ABaseWeapon::StopFire()
{
	// Empty by design. Unreal Engine doesn't support C++ virtual functions in this case. For more info check the PURE_VIRTUAL macro.
}

void ABaseWeapon::Reload(int32 AmountOfAmmunition)
{
	if (Magazine.RemainingAmmunition == Magazine.Capacity)
	{
		return;
	}

	this->SetRemainingAmmunition(AmountOfAmmunition);
	this->ReloadEvent.Broadcast();
	this->User->OnReloadWeapon();
}

void ABaseWeapon::Fire()
{
	if (Magazine.RemainingAmmunition <= 0)
	{
		return;
	}

	UWorld* World = this->GetWorld();
	FVector OutViewPointLocation = FVector::ZeroVector;
	FRotator OutViewPointRotation = FRotator::ZeroRotator;

	this->User->GetViewPoint(OutViewPointLocation, OutViewPointRotation);

	const float WeaponRange = this->Range * 100.0f; // In centimeters.
	const FVector RayDirection = OutViewPointRotation.Vector();
	const FVector RayStart = OutViewPointLocation;
	const FVector RayEnd = RayStart + (RayDirection * WeaponRange);

	FHitResult OutHitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.TraceTag = this->WeaponTraceTag;
	QueryParams.AddIgnoredActor(this->User);
	const bool bHasTraceCollided = World->LineTraceSingleByChannel(OutHitResult, RayStart, RayEnd, WeaponCollisionChannel, QueryParams);

	const FVector TargetLocation = bHasTraceCollided ? OutHitResult.Location : RayEnd;
	const FVector MuzzleLocation = this->Mesh->GetSocketLocation(this->MuzzleSocketName);
	const FVector DirectionFromMuzzleToTarget = TargetLocation - MuzzleLocation;
	const FVector ProjectileDirection = this->Spread > 0.0f ? FMath::VRandCone(DirectionFromMuzzleToTarget, FMath::DegreesToRadians(this->Spread)) : DirectionFromMuzzleToTarget;
	const FRotator ProjectileRotation = FRotationMatrix::MakeFromX(ProjectileDirection).Rotator();

	ABaseProjectile* SpawnedProjectile = World->SpawnActor<ABaseProjectile>(this->Projectile, MuzzleLocation, ProjectileRotation);

	if (SpawnedProjectile)
	{
		SpawnedProjectile->SetWeapon(this);
		UParticleSystem* MuzzleFlashParticle = SpawnedProjectile->GetMuzzleFlashParticle();
		UGameplayStatics::SpawnEmitterAttached(MuzzleFlashParticle, this->Mesh, MuzzleSocketName);
	}

	if (Recoil > 0.0f)
	{
		this->AddRecoil();
	}

	this->Magazine.RemainingAmmunition--;
	this->FireEvent.Broadcast();
	this->User->OnFireWeapon();
}

void ABaseWeapon::AddRecoil()
{
	const float HalfRecoil = this->Recoil / 2.0f;
	const float RandomPitchRecoil = FMath::RandRange(HalfRecoil, Recoil);
	const float RandomYawRecoil = FMath::RandRange(-HalfRecoil, HalfRecoil);

	const FRotator RecoilToAdd = FRotator(RandomPitchRecoil, RandomYawRecoil, 0.0f);
	this->AccumulatedRecoil += RecoilToAdd;

	const FRotator CurrentControlRotation = this->User->GetControlRotation();
	const FRotator RotationAfterRecoil = (CurrentControlRotation + RecoilToAdd).GetNormalized();
	this->User->GetController()->SetControlRotation(RotationAfterRecoil);
}
