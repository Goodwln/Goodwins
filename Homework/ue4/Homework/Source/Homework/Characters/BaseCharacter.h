// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class AInteractiveActor;
class UBaseCharacterMovementComponent;
class UCharacterEquipmentComponent;
class UCharacterAttributeComponent;
struct FDefaultAttributeProperty;

USTRUCT(BlueprintType)
struct FMantlingSettings
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UAnimMontage* MantlingMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	class UCurveVector* MantlingCurveVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorectionXY = 65.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float AnimationCorectionZ = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeight = 200.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeight = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MaxHeightStartTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float MinHeightStartTime = 0.5f;
};


DECLARE_MULTICAST_DELEGATE_OneParam(FOnAimingStateChanged, bool);

UCLASS(Abstract, NotBlueprintable)
class HOMEWORK_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ABaseCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void MoveForward(float Value) {}
	virtual void MoveRight(float Value) {}
	virtual void Turn(float Value) {}
	virtual void LookUp(float Value) {}

	virtual void ChangeCrouchState();
	
	virtual void StartSprint();
	virtual void StopSprint();

	virtual void SwimForward(float Value) {}
	virtual void SwimRight(float Value) {}
	virtual void SwimUp(float Value) {}

	virtual void WallRun(float Value) {}
	
	virtual void StartProne(float HeightAdjust, float ScaledHeightAdjust);

	void ChangeProneState();
	virtual void EndProne(float HeightAdjust, float ScaledHeightAdjust);
	virtual void Jump() override;
	bool IsSwimmingUnderWater();
	void TryChangeOxygenState(float DeltaSeconds);
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void Mantle(bool bForce = false);
	virtual bool CanJumpInternal_Implementation() const override;
	bool CanProne() const;
	
	UBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const { return BaseCharacterMovementComponent; }

	void RegisterInteractiveActor(AInteractiveActor* InteractiveActor);
	void UnRegisterInteractiveActor(class AInteractiveActor* InteractiveActor);
	void InteractWithZipline();
	virtual void StartSlide();
	virtual void StopSlide();
	void OnStartSlide();
	void OnEndSlide();

	UCharacterEquipmentComponent* GetCharacterEquipmentComponent_Mutable() const;
	const UCharacterEquipmentComponent* GetCharacterEquipmentComponent() const;
	void StartFire();
	void StopFire();
	
	void StartAiming();
	void StopAiming();
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStartAiming();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Character")
	void OnStopAiming();

	bool GetIsAiming() const;
	void Reload();
	void NextItem();
	void PreviousItem();
	void EquipPrimaryItem();
	void SecondaryFire();


	FOnAimingStateChanged OnAimingStateChanged;
	
protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Character|Movement")
	void OnSprintStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Character|Movement")
	void OnSprintEnd();

	UFUNCTION(BlueprintImplementableEvent, Category = "Characteer|Movement")
	void OnProneStart(float HeightAdjust, float ScaledHeightAdjust);

	UFUNCTION(BlueprintImplementableEvent, Category = "Characteer|Movement")
	void OnProneEnd(float HeightAdjust, float ScaledHeightAdjust);

	void Prone();
	void UnProne(bool bIsFullHeight);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float BaseTurnRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float BaseLookUpRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float SprintSpeed = 800.0f;

	// Maximum player stamina
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float MaxStamina = 100.f; 

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float CurrentStamina = 50.f;

	// Stamina recovery rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float StaminaRestoreVelocity = 30.f; 

	// Player stamina consumption rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float SprintStaminaConsumptionVelocity = 15.f; 

	// Player stamina consumption rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Controls")
	UCharacterAttributeComponent* CharacterAttributeComponent;

	bool bIsOutStamina = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	class ULedgeDetectorComponent* LedgeDetectorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	class UWallDetectorComponent* WallDetectorComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Movement")
	class UFloorDetectorComponent* FloorDetectorComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling")
	FMantlingSettings HeightMantlingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling")
	FMantlingSettings LowMantlingSettings;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling")
	FMantlingSettings HeightMantlingSettingsFromCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling")
	FMantlingSettings LowMantlingSettingsFromCrouch;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Movement|Mantling", meta = (UIMin = 0.f, ClampMin = 0.f))
	float LowMantleMaxHeight = 125.f;

	bool CanMantle() const;
	void RagdollEnable();

	virtual void OnDeath();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Death")
	UAnimMontage* OnDeathAnimMontage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character | Attributes")
	class UCurveFloat* FallDamageCurve;


	// Player stamina consumption rate
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Controls")
	class UCharacterEquipmentComponent* CharacterEquipmentComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Camera")
	float DefaultFOV = 90.f;

	virtual void OnStartAimingInternal();
	virtual void OnStopAimingInternal();
	
private:
	void TryChangeSprintState(float DeltaSeconds);
	void PerDamage();

	void OnChangeOxygen(float Current, float Max, FDefaultAttributeProperty Attribute);
	void OnChangeStamina(float Current, float Max, FDefaultAttributeProperty Attribute);
	void OnChangeHealth(float Current, float Max, FDefaultAttributeProperty Attribute);
	
	TArray<AInteractiveActor*> AvailableInteractiveActors;

	bool bIsSprintRequested = false;
	bool bIsAiming = false;

	float CurrentAimingMovementSpeed = 0.f;
	
	UPROPERTY()
	UBaseCharacterMovementComponent* BaseCharacterMovementComponent;

	const FMantlingSettings& GetMantlingSettings(float LedgeHeight) const;

	FTimerHandle TimerToDeath;

	FTimerHandle TimerPerDamage;
	
	virtual void Falling() override;
	virtual void NotifyJumpApex() override;
	virtual void Landed(const FHitResult& Hit) override;

	FVector CurrentFallApex = FVector::ZeroVector;
public:

	bool IsAiming();
	float GetCurrentAimingMovementSpeed();
	bool bIsProne = false;

	void ClimbLadderUp(float Value);
	
	void InteractWithLadder();

	const class ALadder* GetAvailableLadder() const;
	const class AZipline* GetAvailableZipline() const;

	class UFloorDetectorComponent* GetFloorDetectorComponent();

	UCharacterAttributeComponent* GetCharacterAttributeComponent() const;

};