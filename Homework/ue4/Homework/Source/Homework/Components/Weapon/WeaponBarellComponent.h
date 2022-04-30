// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkTypes.h"
#include "Net/RepLayout.h"
#include "WeaponBarellComponent.generated.h"

class UNiagaraSystem;
class ABaseProjectile;

UENUM(BlueprintType)
enum class EHitRegistrationType : uint8
{
	None,
	HitScan,
	Projectile
};

USTRUCT(BlueprintType)
struct FDecalInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Decal Info")
	UMaterialInterface* Material;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Decal Info")
	FVector Size = FVector(5.f, 5.f, 5.f);

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Decal Info")
	float LifeTime = 10.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Decal Info")
	float FadeOutTime = 5.f;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Decal Info")
	float FadeScreenSize = 0.001f;
};

USTRUCT(BlueprintType)
struct FWeaponBarrelProperty
{
	GENERATED_BODY()
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Barell attributes")
	float FireRange = 5000.f;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Barell attributes | Damage")
	int32 BulletPerShot = 1;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Barell attributes | Hit Registration")
	EHitRegistrationType HitRegistration = EHitRegistrationType::HitScan;
		
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Barell attributes | Hit Registration", meta = (EditCondition = "HitRegistration == EHitRegistrationType::Projectile"))
	TSubclassOf<ABaseProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Barell attributes | Hit Registration", meta = (UIMin = 1, ClampMin = 1))
	int32 ProjectilePoolSize = 10;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Barell attributes | Damage")
	float DamageAmount = 25.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage")
	UCurveFloat* FalloffDiagram;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | FX")
	UNiagaraSystem* MuzzleFlashFX;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | FX")
	UNiagaraSystem* TraceFX;
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Barell attributes | Decals")
	FDecalInfo DefaultDecalInfo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage")
	TSubclassOf< UDamageType> DamageTypeClass;
 
};

USTRUCT()
struct FReplicationShotInfo
{
	GENERATED_BODY()
	FReplicationShotInfo() : Location_Multi_10(FVector::ZeroVector), Direction(FVector::ZeroVector), SpreadAngle(0.f) {};
	FReplicationShotInfo(FVector Location_In, FVector Direction_In) : Location_Multi_10(Location_In * 10.0f), Direction(Direction_In), SpreadAngle(0.f) {};
	FReplicationShotInfo(FVector Location_In, FVector Direction_In, float SpreadAngle_In) : Location_Multi_10(Location_In * 10.0f), Direction(Direction_In), SpreadAngle(SpreadAngle_In) {};

	UPROPERTY()
	FVector_NetQuantize100 Location_Multi_10;

	UPROPERTY()
	FVector_NetQuantizeNormal Direction;

	UPROPERTY()
	float SpreadAngle;
	
	FVector GetLocation() const { return Location_Multi_10 * 0.1f; }
	FVector GetDirection() const { return Direction; }
	float GetSpreadAngle() const { return  SpreadAngle; }
};

USTRUCT()
struct FInnerArrayProjectilePool
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<ABaseProjectile*> ProjectilePool;
	
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK_API UWeaponBarellComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	UWeaponBarellComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	void Shot(FVector& ShotStart, FVector& ShotDirection, float SpreadAngle);
	float GetBulletPerShot() const;
	EHitRegistrationType GetHitRegistration() const;

	void ChangeWeaponBarrelProperty(EWeaponMode CurrentWeaponMode_In);

	UFUNCTION(Server, Reliable)
	void Server_ChangeWeaponBarrelProperty(EWeaponMode CurrentWeaponMode_In);
	 
	void InitProjectileForCurrentWeapon();
	
	void LaunchProjectile(const FVector& LaunchStart, const FVector& Direction);
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage")
	TMap<EWeaponMode, FWeaponBarrelProperty> AllWeaponBarrelProperty;
	
private:

	UFUNCTION(Reliable,Server)
	void Server_Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle);

	UFUNCTION(Reliable,NetMulticast)
	void NetMulticast_Shot(FVector ShotStart, FVector ShotDirection, float SpreadAngle);

	void ShotReplicated(FVector ShotStart, FVector ShotDirection, float SpreadAngle);
	
	const FVector ProjectilePoolLocation = FVector(0.f, 0.f, -100.f);
	
	UFUNCTION()
	void ProcessProjectileHit(ABaseProjectile* Projectile, const FHitResult& HitResult, const FVector& Direction);

	UPROPERTY(Replicated )
	FWeaponBarrelProperty CurrentWeaponBarrelProperty;
	UPROPERTY(Replicated)
	EWeaponMode CurrentWeaponMode;
	
	UPROPERTY()
	int32 CurrentProjectileIndex;
	UPROPERTY()
	TArray<FInnerArrayProjectilePool> WeaponModeProjectilesPool;
	
	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;

	APawn* GetPawnOwner() const;
	AController* GetController() const;

	void TraceFXPainting(FVector ShotEnd);
	void DrawDecal(FHitResult ShotResult);

	bool HitScan(FVector ShotStart, FVector ShotDirection, float SpreadAngle);
	
	UFUNCTION()
	void ProcessHit(const FHitResult& HitResult, const FVector& Direction);
	void ProjectileScan(FVector& ShotStart, FVector& ShotDirection);

	FVector StartShot = FVector::ZeroVector;

	FTimerHandle TimeDetonation;
	
};





