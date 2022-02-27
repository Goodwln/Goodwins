// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkTypes.h"
#include "WeaponBarellComponent.generated.h"

class UNiagaraSystem;
class ABaseProjectile;

UENUM(BlueprintType)
enum class EHitRegistrationType : uint8
{
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
	TSubclassOf<class UDamageType> DamageTypeClass;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK_API UWeaponBarellComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	void Shot(FVector& ShotStart, FVector& ShotDirection, float SpreadAngle);
	float GetBulletPerShot() const;
	EHitRegistrationType GetHitRegistration() const;

	void ChangeWeaponBarrelProperty(EWeaponMode CurrentWeaponMode);
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Barell attributes | Damage")
	TMap<EWeaponMode, FWeaponBarrelProperty> AllWeaponBarrelProperty;

private:
	FWeaponBarrelProperty CurrentWeaponBarrelProperty;
	
	FVector GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const;

	class APawn* GetPawnOwner() const;
	class AController* GetController() const;

	void TraceFXPainting(FVector ShotEnd);
	void DrawDecal(FHitResult ShotResult);
	bool HitScan(FVector ShotStart, FVector ShotDirection, float SpreadAngle);
	void LaunchProjectile(const FVector& Direction);

	UFUNCTION()
	void ProcessHit(const FHitResult& HitResult, const FVector& Direction);
	void ProjectileScan(FVector& ShotStart, FVector& ShotDirection);

	FVector StartShot = FVector::ZeroVector;
	
};





