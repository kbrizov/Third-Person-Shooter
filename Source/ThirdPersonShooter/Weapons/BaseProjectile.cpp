// Fill out your copyright notice in the Description page of Project Settings.

#include "ThirdPersonShooter/Weapons/BaseProjectile.h"
#include "ThirdPersonShooter/Weapons/BaseWeapon.h"
#include "ThirdPersonShooter/Pawns/BaseCharacter.h"
#include <Kismet/GameplayStatics.h>
#include <Perception/AISense_Damage.h>
#include <Components/SphereComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include <GameFramework/ProjectileMovementComponent.h>

const FName ABaseProjectile::CollisionComponentName = FName(TEXT("Root"));
const FName ABaseProjectile::ParticleSystemComponentName = FName(TEXT("ParticleSystem"));
const FName ABaseProjectile::ProjectileMovementComponentName = FName(TEXT("ProjectileMovement"));

ABaseProjectile::ABaseProjectile(const FObjectInitializer& ObjectInitializer) : 
	Super(ObjectInitializer),
	Damage(100.0f),
	ImpulseForce(8000.0f)
{
	this->InitialLifeSpan = 3.0f;

	this->SetCanBeDamaged(false);

	this->CollisionComponent = this->CreateDefaultSubobject<USphereComponent>(ABaseProjectile::CollisionComponentName);
	this->RootComponent = this->CollisionComponent;

	this->ParticleSystem = this->CreateDefaultSubobject<UParticleSystemComponent>(ABaseProjectile::ParticleSystemComponentName);
	this->ParticleSystem->SetupAttachment(this->RootComponent);

	this->ProjectileMovement = this->CreateDefaultSubobject<UProjectileMovementComponent>(ABaseProjectile::ProjectileMovementComponentName);
	this->ProjectileMovement->UpdatedComponent = this->RootComponent;
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();

	this->CollisionComponent->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnProjectileHit);
}

void ABaseProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	this->CollisionComponent->OnComponentHit.RemoveDynamic(this, &ABaseProjectile::OnProjectileHit);

	Super::EndPlay(EndPlayReason);
}

void ABaseProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComponent, FVector NormalImpulse, const FHitResult& HitResult)
{
	UWorld* World = this->GetWorld();

	if (OtherActor->CanBeDamaged())
	{
		FPointDamageEvent DamageEvent = FPointDamageEvent();
		ABaseCharacter* WeaponUser = this->Weapon->GetUser();
		AController* EventInstigator = WeaponUser->GetController();

		OtherActor->TakeDamage(this->Damage, DamageEvent, EventInstigator, this);
		UAISense_Damage::ReportDamageEvent(World, OtherActor, WeaponUser, this->Damage, WeaponUser->GetActorLocation(), HitResult.ImpactPoint);

		if (OtherComponent->bApplyImpulseOnDamage && OtherComponent->IsSimulatingPhysics())
		{
			FVector Impulse = this->GetActorForwardVector() * this->ImpulseForce;
			OtherComponent->AddImpulseAtLocation(Impulse, HitResult.ImpactPoint, HitResult.BoneName);
		}
	}

	UGameplayStatics::SpawnEmitterAtLocation(World, this->ImpactParticle, this->GetTransform());
	this->Destroy();
}
