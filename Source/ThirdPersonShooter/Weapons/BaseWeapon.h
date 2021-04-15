// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <CoreMinimal.h>
#include <GameFramework/Actor.h>
#include "BaseWeapon.generated.h"

DECLARE_EVENT(ABaseWeapon, FFireEvent)
DECLARE_EVENT(ABaseWeapon, FReloadEvent)

class ABaseProjectile;
class ABaseCharacter;
class USkeletalMeshComponent;

USTRUCT(BlueprintType)
struct FMagazine
{
	GENERATED_BODY()

	/** The maximum amount of ammunition the magazine can store. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Magazine", meta = (ClampMin = "0", UIMin = "0"))
	int32 Capacity = 0;

	/** The ammunition left in the magazine. */
	UPROPERTY(BlueprintReadOnly, Category = "Magazine")
	int32 RemainingAmmunition = 0;
};

UCLASS(Abstract)
class THIRDPERSONSHOOTER_API ABaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	static const FName WeaponTraceTag;
	static const FName MeshComponentName;

	FFireEvent FireEvent;
	FReloadEvent ReloadEvent;

	ABaseWeapon(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void StartFire();

	virtual void StopFire();

	void Reload(int32 AmountOfAmmunition);

	FRotator GetAccumulatedRecoil() const;

	USkeletalMeshComponent* GetMesh() const;

	ABaseCharacter* GetUser() const;

	void SetUser(ABaseCharacter* User);

	float GetRange() const;

	float GetRecoil() const;

	float GetSpread() const;

	int32 GetMagazineCapacity() const;

	int32 GetRemainingAmmunition() const;

	bool IsLoaded() const;

	bool IsMagazineFull() const;

	bool IsEquiped() const;

protected:

	const FName MuzzleSocketName = FName(TEXT("Muzzle"));

	UPROPERTY(EditAnywhere, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TEnumAsByte<ECollisionChannel> WeaponCollisionChannel;

	void Fire();

	void SetRange(float Value);

	void SetRecoil(float Value);

	void SetSpread(float Value);

	void SetMagazineCapacity(int32 Value);

	void SetRemainingAmmunition(int32 Value);

private:	

	FRotator AccumulatedRecoil;

	/** The character currently using the weapon. */
	UPROPERTY(BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	ABaseCharacter* User;

	/** The range of the weapon in meters. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true", ClampMin = "0", UIMin = "0"))
	float Range;

	/** The recoil of the weapon when firing (rotation change in degrees). Used for both Pitch and Yaw. Set to 0 to disable. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "90", UIMin = "0", UIMax = "90"))
	float Recoil;

	/** The spread of the weapon when firing (cone half angle in degrees). Set to 0 to disable. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true", ClampMin = "0", ClampMax = "90", UIMin = "0", UIMax = "90"))
	float Spread;

	/** The magazine holding the ammunition for the weapon. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	FMagazine Magazine;

	/** The type of the projectile to be spawned by the weapon. */
	UPROPERTY(EditDefaultsOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<ABaseProjectile> Projectile;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon", meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;

	void AddRecoil();
};

inline FRotator ABaseWeapon::GetAccumulatedRecoil() const
{
	return this->AccumulatedRecoil;
}

inline USkeletalMeshComponent* ABaseWeapon::GetMesh() const
{
	return this->Mesh;
}

inline ABaseCharacter* ABaseWeapon::GetUser() const
{
	return this->User;
}

inline void ABaseWeapon::SetUser(ABaseCharacter* User)
{
	this->User = User;
}

inline float ABaseWeapon::GetRange() const
{
	return this->Range;
}

inline void ABaseWeapon::SetRange(float Value)
{
	check(0.0f <= Value);
	this->Range = Value;
}

inline float ABaseWeapon::GetRecoil() const
{
	return this->Recoil;
}

inline void ABaseWeapon::SetRecoil(float Value)
{
	check(0.0f <= Value && Value <= 90.0f);
	this->Recoil = Value;
}

inline float ABaseWeapon::GetSpread() const
{
	return this->Spread;
}

inline void ABaseWeapon::SetSpread(float Value)
{
	check(0.0f <= Value && Value <= 90.0f);
	this->Spread = Value;
}

inline int32 ABaseWeapon::GetMagazineCapacity() const
{
	return this->Magazine.Capacity;
}

inline void ABaseWeapon::SetMagazineCapacity(int32 Value)
{
	check(0.0f <= Value);
	this->Magazine.Capacity = Value;
}

inline int32 ABaseWeapon::GetRemainingAmmunition() const
{
	return this->Magazine.RemainingAmmunition;
}

inline void ABaseWeapon::SetRemainingAmmunition(int32 Value)
{
	this->Magazine.RemainingAmmunition = FMath::Clamp(Value, 0, this->Magazine.Capacity);
}

inline bool ABaseWeapon::IsLoaded() const
{
	bool bIsLoaded = (this->Magazine.RemainingAmmunition > 0);

	return bIsLoaded;
}

inline bool ABaseWeapon::IsMagazineFull() const
{
	bool bIsMagazineFull = (this->Magazine.RemainingAmmunition == this->Magazine.Capacity);

	return bIsMagazineFull;
}

inline bool ABaseWeapon::IsEquiped() const
{
	bool bIsEquiped = (this->User != nullptr);

	return bIsEquiped;
}
