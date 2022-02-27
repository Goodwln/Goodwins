// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EqupableItem.h"
#include "RangeWeaponItem.generated.h"


UENUM(BlueprintType)
enum class EWeaponFireMode : uint8
{
	Single,
	FullAuto
};

UENUM(BlueprintType)
enum class EReloadMode : uint8
{
	FullClip,
	PerBullet
};

USTRUCT(BlueprintType)
struct FWeaponModeProperty
{
	GENERATED_BODY()

	/**
	 * How currect to use this Enum
	 * @FullClip - used when need give full Clip for range weapon
	 * @PerBullet - used when need give on one bullet for range weapon
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters")
	EReloadMode ReloadMode = EReloadMode::FullClip;
	
	// Rate of fire in rounds per min
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters", meta = (ClampMin = 1.f, UIMin = 1.f))
	float RateOfFire = 600.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters")
	EWeaponFireMode WeaponFireMode = EWeaponFireMode::Single;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 2.f, UIMax = 2.f))
	float SpreadAngle = 2.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters", meta = (ClampMin = 0.f, UIMin = 0.f, ClampMax = 2.f, UIMax = 2.f))
	float AimSpreadAngle = 0.25f;
	               
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters")
	EAmunitionType AmmoType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters", meta = (ClampMin = 1, UIMin = 1))
	int32 AmmoMax = 30;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	UAnimMontage* WeaponReloadMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
	UAnimMontage* CharacterReloadMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Weapon")
	UAnimMontage* WeaponFireMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Animations | Character")
	UAnimMontage* CharacterFireMontage;
	
	UPROPERTY()
	int32 CurrentAmmo = 0;

	UPROPERTY()
	int32 SaveAmmo = 0;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAmmoChange, int32);
DECLARE_MULTICAST_DELEGATE(FOnReloadComplete);
/**
 * 
 */
UCLASS(Blueprintable)
class HOMEWORK_API ARangeWeaponItem : public AEqupableItem
{
	GENERATED_BODY()

public:

	
	ARangeWeaponItem();

	void StartFire();
	void StopFire();

	void StartAim();
	void StopAim();

	void StartReload();
	void EndReload(bool bIsSuccess);
	
	float GetAimFOV();
	UCurveFloat* GetFOVCurve();

	float GetAimMovementSpeed() const;
	
	FTransform GetForeGripTransform() const;

	float GetAimTurnModifier() const;

	float GetAimLookUpModifier() const;

	int32 GetMaxAmmo() const;
	int32 GetAmmo() const;
	void SetAmmo(int NewAmmo);
	bool CanShoot() const;

	EAmunitionType GetAmunitionType() const;

	virtual void SecondaryFire();

	FOnAmmoChange OnAmmoChange;
	FOnReloadComplete OnReloadComplete;

	virtual  ECurrentReticle GetCurrentReticle() const override;


protected:

	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USkeletalMeshComponent* WeaponMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UWeaponBarellComponent* WeaponBarell;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters | Weapon Mode")
	EWeaponMode StartingWeaponMode;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters | Weapon Mode")
	TMap<EWeaponMode, FWeaponModeProperty> AllWeaponModeProperty;
	  
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters")
	bool bAutoReload = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters", meta = (ClampMin = 1.f, UIMin = 1.f, ClampMax = 2.f, UIMax = 2.f))
	float AimMovementSpeed = 200.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters", meta = (ClampMin = 1.f, UIMin = 1.f, ClampMax = 160.f, UIMax = 160.f))
	float AimFOV = 60.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters")
	UCurveFloat* FOVCurve;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters")
	float AimTurnModifier = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon | Paraneters")
	float AimLookUpModifier = 0.3f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
	ECurrentReticle AimCurrentReticle = ECurrentReticle::None;

	float GetCurrentBulletSpreadAngle() const;
private:


	bool bIsReloading = false;
	bool bIsFireEnable = false;
	
	bool bIsAiming = false;
	
	void MakeShot();
	float GetShotTimerInterval() const;
	
	float PlayAnimMontage(UAnimMontage* AnimMontage);
	void StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime = 0.f);
	
	bool bIsFiring = false;
	
	FTimerHandle ShotTimer;
	FTimerHandle ReloadTimer;

	void OnShotTimerElapsed();

	void InitializeWeaponMode();
	FWeaponModeProperty CurrentWeaponModeProperty;
	EWeaponMode CurrentWeaponMode;

	FWeaponModeProperty NextWeaponMode(bool bWasCurrentKey);
};
	