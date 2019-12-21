// Fill out your copyright notice in the Description page of Project Settings.

#include "BaseCharacter.h"
#include "BaseWeapon.h"
#include "CoverBoxComponent.h"
#include "CoverEndComponent.h"
#include "CharacterLocomotionComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Gameframework/Controller.h"

const FName ABaseCharacter::FocalPointComponentName = FName(TEXT("FocalPoint"));
const FName ABaseCharacter::WeaponComponentName = FName(TEXT("WeaponActor"));

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer.SetDefaultSubobjectClass<UCharacterLocomotionComponent>(ACharacter::CharacterMovementComponentName)),
	bIsWalking(false),
	bIsRunning(true), // Starts in running mode by default (Uses run movement speed).
	bIsFiring(false),
	bIsReloading(false),
	bIsInCover(false),
	OverlappingCoverVolume(nullptr)
{
	this->SetMaxHealth(1000.0f);
	this->SetCurrentHealth(this->MaxHealth);

	this->FocalPoint = this->CreateDefaultSubobject<USceneComponent>(ABaseCharacter::FocalPointComponentName);
	this->FocalPoint->SetupAttachment(this->GetMesh());
	this->FocalPoint->SetRelativeLocation(FVector(0.0f, 0.0f, 125.0f));

	this->Weapon = this->CreateDefaultSubobject<UChildActorComponent>(ABaseCharacter::WeaponComponentName);
	this->Weapon->SetupAttachment(this->GetMesh(), this->RightHandSocketName);
}

float ABaseCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	//UE_LOG(LogTemp, Warning, TEXT("I'm taking %f damage!"), Damage);
	
	const float NewHealth = CurrentHealth - Damage;
	this->SetCurrentHealth(NewHealth);

	if (NewHealth < 0.0f || FMath::IsNearlyZero(NewHealth))
	{
		this->Die();
	}

	return Damage;
}

void ABaseCharacter::FireWeaponStart()
{
	if (!CanFireWeapon())
	{
		return;
	}
	
	ABaseWeapon* EquipedWeapon = this->GetEquipedWeapon();
	check(EquipedWeapon);
	
	EquipedWeapon->StartFire();
	this->bIsFiring = true;
}

void ABaseCharacter::FireWeaponStop()
{
	if (!IsFiring())
	{
		return;
	}

	ABaseWeapon* equipedWeapon = this->GetEquipedWeapon();
	check(equipedWeapon);

	equipedWeapon->StopFire();
	this->bIsFiring = false;
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
	check(EquipedWeapon);

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
	check(NewWeapon);

	this->Weapon->SetChildActorClass(NewWeapon->GetClass());

	ABaseWeapon* Weapon = Cast<ABaseWeapon>(this->Weapon->GetChildActor());
	check(Weapon);

	Weapon->SetUser(this);

	// SetChildActorClass creates a copy of the NewWeapon, so it's not needed anymore.
	NewWeapon->Destroy();
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
		const FVector ControlForward = this->GetControlRotation().Vector();
		const FVector CoverVolumeForward = this->OverlappingCoverVolume->GetForwardVector();
		const float DotProduct = FVector::DotProduct(ControlForward, CoverVolumeForward);

		bIsFacingCover = DotProduct > 0.0f;
	}

	return bIsFacingCover;
}

bool ABaseCharacter::CanTakeCover() const
{
	const bool bIsInCoverVolume = this->OverlappingCoverVolume != nullptr;
	const bool bIsFacingCover = this->IsFacingCover();

	const bool bCanTakeCover = bIsInCoverVolume && bIsFacingCover && !bIsFiring && !bIsReloading;

	return bCanTakeCover;
}

bool ABaseCharacter::CanExitCover() const
{
	const bool bIsInCoverVolume = this->OverlappingCoverVolume != nullptr;

	const bool bCanExitCover = bIsInCoverVolume && !bIsFiring && !bIsReloading;

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
