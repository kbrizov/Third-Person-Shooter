// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseWeapon.h"
#include "BaseCharacter.h"
#include "BaseProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"

const FName ABaseWeapon::WeaponTraceTag = FName(TEXT("WeaponTrace"));
const FName ABaseWeapon::MeshComponentName = FName(TEXT("Mesh"));

ABaseWeapon::ABaseWeapon(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	this->AccumulatedRecoil = FRotator::ZeroRotator;

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
	this->User->ReloadWeaponEvent.Broadcast();
}

FRotator ABaseWeapon::GetAccumulatedRecoil() const
{
	return this->AccumulatedRecoil;
}

USkeletalMeshComponent* ABaseWeapon::GetMesh() const
{
	return this->Mesh;
}

ABaseCharacter* ABaseWeapon::GetUser() const
{
	return this->User;
}

void ABaseWeapon::SetUser(ABaseCharacter* User)
{
	this->User = User;
}

float ABaseWeapon::GetRange() const
{
	return this->Range;
}

void ABaseWeapon::SetRange(float Value)
{
	this->Range = Value < 0.0f ? 0.0f : Value;
}

float ABaseWeapon::GetRecoil() const
{
	return this->Recoil;
}

void ABaseWeapon::SetRecoil(float Value)
{
	this->Recoil = FMath::Clamp(Value, 0.0f, 90.0f);
}

float ABaseWeapon::GetSpread() const
{
	return this->Spread;
}

void ABaseWeapon::SetSpread(float Value)
{
	this->Spread = FMath::Clamp(Value, 0.0f, 90.0f);
}

int32 ABaseWeapon::GetMagazineCapacity() const
{
	return this->Magazine.Capacity;
}

void ABaseWeapon::SetMagazineCapacity(int32 Value)
{
	this->Magazine.Capacity = Value < 0.0f ? 0.0f : Value;
}

int32 ABaseWeapon::GetRemainingAmmunition() const
{
	return this->Magazine.RemainingAmmunition;
}

void ABaseWeapon::SetRemainingAmmunition(int32 Value)
{
	this->Magazine.RemainingAmmunition = FMath::Clamp(Value, 0, this->Magazine.Capacity);
}

bool ABaseWeapon::IsLoaded() const
{
	bool bIsLoaded = (this->Magazine.RemainingAmmunition > 0);

	return bIsLoaded;
}

bool ABaseWeapon::IsMagazineFull() const
{
	bool bIsMagazineFull = (this->Magazine.RemainingAmmunition == this->Magazine.Capacity);

	return bIsMagazineFull;
}

bool ABaseWeapon::IsEquiped() const
{
	bool bIsEquiped = (this->User != nullptr);

	return bIsEquiped;
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
	bool bHasTraceCollided = World->LineTraceSingleByChannel(OutHitResult, RayStart, RayEnd, WeaponCollisionChannel, QueryParams);

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
	this->User->FireWeaponEvent.Broadcast();
}

void ABaseWeapon::AddRecoil()
{
	float HalfRecoil = this->Recoil / 2.0f;
	float RandomPitchRecoil = FMath::RandRange(HalfRecoil, Recoil);
	float RandomYawRecoil = FMath::RandRange(-HalfRecoil, HalfRecoil);

	FRotator RecoilToAdd = FRotator(RandomPitchRecoil, RandomYawRecoil, 0.0f);
	this->AccumulatedRecoil += RecoilToAdd;

	FRotator CurrentControlRotation = this->User->GetControlRotation();
	FRotator RotationAfterRecoil = (CurrentControlRotation + RecoilToAdd).GetNormalized();
	this->User->GetController()->SetControlRotation(RotationAfterRecoil);
}
