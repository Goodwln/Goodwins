// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkTypes.h"
#include "Components/AIComponents/ItemAttributeComponent.h"
#include "GameFramework/Pawn.h"
#include "Turret.generated.h"


UENUM(BlueprintType)
enum class ETurretState : uint8
{
	None,
	Searching,
	Tracking
};

class USceneComponent;
class UWeaponBarellComponent;
class UNiagaraSystem;
UCLASS()
class HOMEWORK_API ATurret : public APawn
{
	GENERATED_BODY()

public:
	ATurret();
	
	virtual void Tick(float DeltaTime) override;
	virtual void BeginPlay() override;
	virtual void PossessedBy(AController* NewController) override;
	
	void OnCurrentTargetSet();

	virtual FVector GetPawnViewLocation() const override;

	virtual FRotator GetViewRotation() const override;

	bool IsDestroy() const;

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY( )
	AActor* CurrentTarget = nullptr;

 protected:
	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Components")
	USceneComponent* TurretBaseComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Components")
	USceneComponent* TurretBarrelComponent;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly, Category = "Components")
	UWeaponBarellComponent* WeaponBarrel;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurretParameters", meta = (ClampMin = 0.f, CLampUI = 0.f))
	float BaseSearchingRotationRate = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurretParameters", meta = (ClampMin = 0.f, CLampUI = 0.f))
	float BarrelPitchRotationRate = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurretParameters", meta = (ClampMin = 0.f, CLampUI = 0.f))
	float MaxBarrelPitchAngle = 60.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurretParameters", meta = (ClampMin = 0.f, CLampUI = 0.f))
	float MinBarrelPitchAngle = -30.f;  

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "TurretParameters", meta = (ClampMin = 0.f, CLampUI = 0.f))
	float BaseTrackingInterpSpeed = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Fire", meta = (ClampMin = 1.f, CLampUI = 1.f))
	float RateOfFire = 300.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Fire", meta = (ClampMin = 0.f, CLampUI = 0.f))
	float BulletSpreadAngle = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Fire", meta = (ClampMin = 0.f, CLampUI = 0.f))
	float FireDelayTime = 3.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret | Team")
	ETeams Team = ETeams::None;
	void MakeShot();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Attribute")
	UItemAttributeComponent* AttributeComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Turret parameters | Attribute | Death Property")
	UParticleSystem* DestroyParticle;

	UFUNCTION()
	virtual void OnDeath();

private:
 
	
	void SearchingMovement(float DeltaTime);
	
	void TrackingMovement(float DeltaTime);

	void SetCurrentTurretState(ETurretState NewState);
	ETurretState CurrentTurretState = ETurretState::None;



	float GetFireInterval() const;
	
	FTimerHandle ShotTimer;

	
};
