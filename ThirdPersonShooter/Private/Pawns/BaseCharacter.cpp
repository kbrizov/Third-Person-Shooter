// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "BaseWeapon.h"
#include "CoverBoxComponent.h"
#include "CoverEndComponent.h"
#include "CharacterLocomotionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"

const FName ABaseCharacter::FocalPointComponentName = FName(TEXT("FocalPoint"));
const FName ABaseCharacter::WeaponComponentName = FName(TEXT("WeaponActor"));

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterLocomotionComponent>(ACharacter::CharacterMovementComponentName))
{
	this->bIsWalking = false;
	this->bIsRunning = true; // Starts in running mode by default (Uses run movement speed).
	this->bIsFiring = false;
	this->bIsReloading = false;
	this->bIsInCover = false;
	this->OverlappingCoverVolume = nullptr;
	this->SetMaxHealth(1000.0f);
	this->SetCurrentHealth(this->MaxHealth);

	USkeletalMeshComponent* MeshComponent = this->GetMesh();
	this->FocalPoint = this->CreateDefaultSubobject<USceneComponent>(ABaseCharacter::FocalPointComponentName);
	this->FocalPoint->SetupAttachment(MeshComponent);
	this->FocalPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 125.0f));

	this->Weapon = this->CreateDefaultSubobject<UChildActorComponent>(ABaseCharacter::WeaponComponentName);
	this->Weapon->SetupAttachment(this->GetMesh(), this->RightHandSocketName);
}

float ABaseCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	UE_LOG(LogTemp, Warning, TEXT("I'm taking %f damage!"), Damage);
	
	float NewHealth = CurrentHealth - Damage;
	this->SetCurrentHealth(NewHealth);

	if (FMath::IsNearlyZero(NewHealth))
	{
		this->Die();
	}

	return Damage;
}

void ABaseCharacter::Aim()
{
	this->bIsAiming = true;
}

void ABaseCharacter::AimStop()
{
	this->bIsAiming = false;
}

void ABaseCharacter::FireWeapon()
{
	if (!CanFireWeapon())
	{
		return;
	}

	this->bIsFiring = true;
	this->GetEquipedWeapon()->StartFire();
}

void ABaseCharacter::FireWeaponStop()
{
	if (!IsFiring())
	{
		return;
	}

	this->bIsFiring = false;
	this->GetEquipedWeapon()->StopFire();
}

void ABaseCharacter::ReloadWeapon()
{
	if (!CanReloadWeapon())
	{
		return;
	}

	// TODO: Calculate reload variables. (How much ammo we have, how much ammo must be reloaded etc.)
	// Temporary solution below (Infinite ammo):
	ABaseWeapon* EquipedWeapon = this->GetEquipedWeapon();
	EquipedWeapon->Reload(EquipedWeapon->GetMagazineCapacity());
}

FVector ABaseCharacter::GetFocalPoint() const
{
	FVector FocalPointLocation = FVector::ZeroVector;

	if (FocalPoint)
	{
		FocalPointLocation = this->FocalPoint->GetComponentTransform().GetLocation();
	}
	else
	{
		FocalPointLocation = this->GetTransform().GetLocation();
	}

	return FocalPointLocation;
}

void ABaseCharacter::GetViewPoint(FVector& OutLocation, FRotator& OutRotation) const
{
	AController* Controller = this->GetController();
	
	if (Controller)
	{
		Controller->GetPlayerViewPoint(OutLocation, OutRotation);
	}
}

ABaseWeapon* ABaseCharacter::GetEquipedWeapon() const
{
	ABaseWeapon* EquipedWeapon = nullptr;

	if (Weapon)
	{
		EquipedWeapon = Cast<ABaseWeapon>(this->Weapon->GetChildActor());
	}

	return EquipedWeapon;
}

void ABaseCharacter::EquipWeapon(ABaseWeapon* NewWeapon)
{
	checkf(NewWeapon != nullptr, TEXT("Weapon is null."));

	this->Weapon->SetChildActorClass(NewWeapon->GetClass());
	ABaseWeapon* Weapon = Cast<ABaseWeapon>(this->Weapon->GetChildActor());
	Weapon->SetUser(this);

	// SetChildActorClass creates a copy of the NewWeapon, so it's not needed anymore.
	Destroy(NewWeapon);
}

float ABaseCharacter::GetMaxHealth() const
{
	return this->MaxHealth;
}

void ABaseCharacter::SetMaxHealth(float Value)
{
	this->MaxHealth = Value < 0.0f ? 0.0f : Value;
}

float ABaseCharacter::GetCurrentHealth() const
{
	return this->CurrentHealth;
}

void ABaseCharacter::SetCurrentHealth(float Value)
{
	this->CurrentHealth = FMath::Clamp(Value, 0.0f, MaxHealth);
}

float ABaseCharacter::GetCurrentHealthNormalized() const
{
	// NormalizedHealth is in [0, 1]
	// CurrentHealth is in [0, MaxHealth]
	// The ratio: NormalizedHealth / 1 = CurrentHealth / MaxHealth 
	float NormalizedHealth = this->CurrentHealth / this->MaxHealth;

	return NormalizedHealth;
}

UCoverBoxComponent* ABaseCharacter::GetOverlappingCoverVolume() const
{
	return this->OverlappingCoverVolume;
}

UCoverEndComponent* ABaseCharacter::GetOverlappingCoverEndVolume() const
{
	return this->OverlappingCoverEndVolume;
}

bool ABaseCharacter::IsIdle() const
{
	bool bIsMoving = this->GetVelocity().SizeSquared() > 0.0f;

	return !bIsMoving;
}

bool ABaseCharacter::IsWalking() const
{
	return this->bIsWalking;
}

bool ABaseCharacter::IsRunning() const
{
	return this->bIsRunning;
}

bool ABaseCharacter::IsFiring() const
{
	return this->bIsFiring;
}

void ABaseCharacter::SetIsFiring(bool bValue)
{
	this->bIsFiring = bValue;
}

bool ABaseCharacter::IsReloading() const
{
	return this->bIsReloading;
}

void ABaseCharacter::SetIsReloading(bool bValue)
{
	this->bIsReloading = bValue;
}

bool ABaseCharacter::IsInCover() const
{
	return this->bIsInCover;
}

bool ABaseCharacter::IsDead() const
{
	bool bIsAlive = this->CurrentHealth > 0.0f;

	return !bIsAlive;
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	ABaseWeapon* EquipedWeapon = this->GetEquipedWeapon();

	if (EquipedWeapon)
	{
		EquipedWeapon->SetUser(this);
		EquipedWeapon->SetActorEnableCollision(false);
	}

	UCapsuleComponent* CapsuleComponent = this->GetCapsuleComponent();
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::OnCoverVolumeBeginOverlap);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::OnCoverVolumeEndOverlap);
	CapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &ABaseCharacter::OnCoverEndVolumeBeginOverlap);
	CapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &ABaseCharacter::OnCoverEndVolumeEndOverlap);
}

void ABaseCharacter::Die()
{
	ABaseWeapon* EquipedWeapon = this->GetEquipedWeapon();

	if (EquipedWeapon)
	{
		if (IsFiring())
		{
			this->FireWeaponStop();
		}

		EquipedWeapon->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		EquipedWeapon->SetActorEnableCollision(true);
		EquipedWeapon->GetMesh()->SetSimulatePhysics(true);
	}

	UCapsuleComponent* CapsuleComponent = this->GetCapsuleComponent();
	CapsuleComponent->Deactivate();
	CapsuleComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	this->GetCharacterMovement()->Deactivate();
	this->GetMesh()->SetSimulatePhysics(true);

	this->DeathEvent.Broadcast();
}

bool ABaseCharacter::IsFacingCover() const
{
	bool bIsFacingCover = false;

	if (OverlappingCoverVolume)
	{
		FVector ControlForward = this->GetControlRotation().Vector();
		FVector CoverVolumeForward = this->OverlappingCoverVolume->GetForwardVector();
		float DotProduct = FVector::DotProduct(ControlForward, CoverVolumeForward);

		bIsFacingCover = DotProduct > 0.0f;
	}

	return bIsFacingCover;
}

bool ABaseCharacter::CanTakeCover() const
{
	bool bIsInCoverVolume = this->OverlappingCoverVolume != nullptr;
	bool bIsFacingCover = this->IsFacingCover();

	bool bCanTakeCover = bIsInCoverVolume && bIsFacingCover && !bIsFiring && !bIsReloading;

	return bCanTakeCover;
}

bool ABaseCharacter::CanExitCover() const
{
	bool bIsInCoverVolume = this->OverlappingCoverVolume != nullptr;

	bool bCanExitCover = bIsInCoverVolume && !bIsFiring && !bIsReloading;

	return bCanExitCover;
}

bool ABaseCharacter::CanFireWeapon() const
{
	bool bCanFireWeapon = false;

	if (!IsDead())
	{
		ABaseWeapon* EquipedWeapon = this->GetEquipedWeapon();

		if (EquipedWeapon)
		{
			bCanFireWeapon = EquipedWeapon->IsLoaded() && !bIsReloading;
		}
	}

	return bCanFireWeapon;
}

bool ABaseCharacter::CanReloadWeapon() const
{
	// TODO: Check if the player can reload (Has ammo available etc).
	bool bCanReloadWeapon = false;

	if (!IsDead())
	{
		ABaseWeapon* EquipedWeapon = this->GetEquipedWeapon();

		if (EquipedWeapon)
		{
			bCanReloadWeapon = !EquipedWeapon->IsMagazineFull() && !bIsFiring && !bIsReloading;
		}
	}

	return bCanReloadWeapon;
}

void ABaseCharacter::OnCoverVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UCoverBoxComponent* CoverVolume = Cast<UCoverBoxComponent>(OtherComponent);

	if (CoverVolume == nullptr)
	{
		return;
	}

	this->OverlappingCoverVolume = CoverVolume;
}

void ABaseCharacter::OnCoverVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	UCoverBoxComponent* CoverVolume = Cast<UCoverBoxComponent>(OtherComponent);

	if (CoverVolume == nullptr)
	{
		return;
	}

	this->OverlappingCoverVolume = nullptr;
}

void ABaseCharacter::OnCoverEndVolumeBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UCoverEndComponent* EndVolume = Cast<UCoverEndComponent>(OtherComponent);

	if (EndVolume == nullptr)
	{
		return;
	}

	this->OverlappingCoverEndVolume = EndVolume;
}

void ABaseCharacter::OnCoverEndVolumeEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, int32 OtherBodyIndex)
{
	UCoverEndComponent* EndVolume = Cast<UCoverEndComponent>(OtherComponent);

	if (EndVolume == nullptr)
	{
		return;
	}

	this->OverlappingCoverEndVolume = nullptr;
}

