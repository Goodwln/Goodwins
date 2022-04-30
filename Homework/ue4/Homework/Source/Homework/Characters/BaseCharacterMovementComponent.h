// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "../Components/DetectorComponents/LedgeDetectorComponent.h"
#include "Components/TimelineComponent.h"
#include "Homework/Actors/Interactive/Enviroment/Zipline.h"
#include "Homework/Components/DetectorComponents/WallDetectorComponent.h"
#include "BaseCharacterMovementComponent.generated.h"

class ABaseCharacter;

struct FMantlingMovementParameters
{
	FVector InitialLocation = FVector::ZeroVector;
	FRotator InitialRotation = FRotator::ZeroRotator;

	FVector TargetLocation = FVector::ZeroVector;
	FRotator TargetRotation = FRotator::ZeroRotator;
	
	FVector InitialAnimationLocation = FVector::ZeroVector;
	
	 UPrimitiveComponent* ActorTargetLedge;

	FVector SaveLocation = FVector::ZeroVector;
	
	FVector MantlingOffset = FVector::ZeroVector;
	
	float Duration = 1.f;
	float StartTime = 0.f;

	UCurveVector* MantlingCurve;
};


struct FWallRunParameters
{
	ESide Side = ESide::None;
	float CurrentWallRunSide = 0.f;
	float Angle = 0.f;
};

struct  FSlideParameters
{
	float Angle = 0.f;
	
};

UENUM(BlueprintType)
enum class ECustomMovementMode : uint8
{
	CMOVE_None = 0 UMETA(DisplayName = "None"),
	CMOVE_Zipline UMETA(DisplayName = "Zipline"),
	CMOVE_Ladder UMETA(DisplayName = "Ladder"),
	CMOVE_Mantling UMETA(DisplayName = "Mantle"),
	CMOVE_WallRun UMETA(DisplayName = "WallRun"),
	CMOVE_Slide UMETA(DisplayName = "Slide"),
	CMOVE_Max UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EDetachFromLadderMethod : uint8
{
	Fall = 0,
	ReachingTheTop,
	ReachingTheBottom,
	JumpOff
};

UENUM(BlueprintType)
enum class EEndingWallRunMethod : uint8
{
	Fall = 0,
	JumpOff
};

UENUM(BlueprintType)
enum class ECurrentCapsuleSize : uint8
{
	None = 0,
	DefaultCapsule,
	CrouchCapsule,
	ProneCapsule,
	SlideCapsule
};

USTRUCT(BlueprintType)
struct FCapsuleSize
{
	GENERATED_BODY()

	FCapsuleSize() {}
	FCapsuleSize(float OutHalfHeight, float OutRadius) : HalfHeight(OutHalfHeight), Radius(OutRadius) { }
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMax = 0.f));
	float HalfHeight = 0.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.f, UIMax = 0.f));
	float Radius = 0.f;
};

UCLASS()
class HOMEWORK_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	friend class FSaveMove_BaseCharacter;

	
public:
	UBaseCharacterMovementComponent();

	FORCEINLINE bool IsSprinting() { return bIsSprinting; }

	virtual float GetMaxSpeed() const override;

	virtual void Prone(bool bClientSimulation = false);
	virtual void UnProne(bool bClientSimulation = false);
	
	UFUNCTION(BlueprintCallable, Category = "Prone")
	bool IsProne() const;

	bool CanProneInCurrentState() const;

	void StartSprint();

	void StopSprint();

	void StartMantle(const FMantlingMovementParameters& MantlingParameters);
	void EndMantle();
	bool IsMantling() const;

	UFUNCTION(BlueprintCallable, Category = "Prone")
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);

	UFUNCTION(BlueprintCallable, Category = "Prone")
	FORCEINLINE bool GetIsOutOfStamina() { return bIsOutOfStamina; };

	void AttachToLadder(const class ALadder* Ladder);
	float GetActorToCurrentLadderProjection(const FVector& Location) const;
	void DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod = EDetachFromLadderMethod::Fall);
	bool IsOnLadder() const;
	const class ALadder* GetCurrentLadder();
	void PhysLadder(float deltaTime, int32 Iterations);
	float GetLadderSpeedRatio() const;

	FVector GetLocationCharacterToZipline();
	void AttachToZipline(const class AZipline* Zipline);
	void DetachFromZipline();
	bool IsOnZipline() const;
	void PhysZipline(float DeltaTime, int32 Iterations);

	void WallRunStart(FWallRunParameters& WallRunParameters);
	void LaunchJumpDirection(FVector JumpDirection);

	UFUNCTION()
	void WallRunEnd(EEndingWallRunMethod EndingWallRun);
	bool IsOnWallRun() const;
	void PhysWallRun(float DeltaTime, int32 Iterations);

	void StartSlide(FSlideParameters& OutSlideParameters);
	
	void StopSlide();

	bool GetIsSprinting() const { return  bIsSprinting; }
protected:
	// Capsule radius in a prone state
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Prone", meta = (ClampMin = 1.f))
	float ProneCapsuleRadius = 40.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Sprint", meta = (ClampMin = 0.f, UIMin = 0.f))
	float SprintSpeed = 1600;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Stamina", meta = (ClampMin = 0.f, UIMin = 0.f))
	float OutStaminaWalkSpeed = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Prone", meta = (ClampMin = 0.f, UIMax = 0.f))
	float MaxProneSpeed = 100.0f;

	// Capsule half height in prone state
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Prone", meta = (ClampMin = 1.f))
	float ProneCapsuleHalfHeight = 40.f; 

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Ladder", meta = (ClampMin = 0.f, UIMax = 0.f))
	float MaxClimbingOnLadderSpeed = 200.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Ladder", meta = (ClampMin = 0.f, UIMax = 0.f))
	float LadderToCharacterOffset = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Ladder", meta = (ClampMin = 0.f, UIMax = 0.f))
	float ClimbingOnLadderBrakingDeceleration = 2048.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Ladder", meta = (ClampMin = 0.f, UIMax = 0.f))
	float MaxLadderTopOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Ladder", meta = (ClampMin = 0.f, UIMax = 0.f))
	float MinLadderBottomOffset = 90.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Ladder", meta = (ClampMin = 0.f, UIMax = 0.f))
	float JumpOffFromLadderSpeed = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Swimming", meta = (ClampMin = 0.f, UIMax = 0.f))
	float SwimmingCapsuleRadius = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Swimming", meta = (ClampMin = 0.f, UIMax = 0.f))
	float SwimmingCapsuleHalfSize = 60.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Swimming", meta = (ClampMin = 0.f, UIMax = 0.f))
	float SwimmingMaxSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Zipline", meta = (ClampMin = 0.f, UIMax = 0.f))
	float CharacterSpeedToZipline = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Zipline", meta = (ClampMin = 0.f, UIMax = 0.f))
	float CharacterWallRunningSpeed = 500.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Zipline", meta = (ClampMin = 0.f, UIMax = 0.f))
	float CharacterUnderZiplineOffset = 36.f;

	//*Offset when detach*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Zipline", meta = (ClampMin = 0.f, UIMax = 0.f))
	float PillarXOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement| WallRun", meta = (ClampMin = 0.f, UIMax = 0.f))
	bool bIsVerticalChangeWallRun = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement| WallRun", meta = (ClampMin = 0.f, UIMax = 0.f))
	bool bIsEndlessWallRun = false;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement| WallRun", meta = (ClampMin = 0.f, UIMax = 0.f))
	float MaxWallRunTime = 2.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character movement| WallRun", meta = (ClampMin = 0.f, UIMax = 0.f))
	float MaxWallRunSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement| WallRun", meta = (ClampMin = 0.f, UIMax = 0.f))
	float JumpOffFromWallSpeed = 300.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Movement| Slide",  meta=(ClampMin="0", UIMin="0"))
	float SlideSpeed = 1000.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Movement")// List Sizes capsule for states character 
	TMap<ECurrentCapsuleSize,FCapsuleSize> CapsuleSizeList;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Character Movement: Slide", meta=(ClampMin="0", UIMin="0"))
	float SlideMaxTime = 2.0f;
	
	ABaseCharacter* GetBaseCharacterOwner() const;

	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;
	virtual void OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode) override;
	virtual void PhysCustom(float DeltaTime, int32 Iterations) override;

	void PhysMantling(float DeltaTime, int32 Iterations);

	void MoveUpdateMantling(FVector CorrectedInitialLocation, FVector &TargetUpdateLocation, float PositionAlpha, float DeltaTime);

	virtual void PhysicsRotation(float DeltaTime) override;
	void InitTimelineWallRunSideRotation();
	void InitTimelineWallRunGrowingWeight();
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement| WallRun")
	UCurveFloat *RotateWallRun;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement| WallRun")
	UCurveFloat *GrowingWeight;

	
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
private:
	
	bool bIsSprinting = false;
	bool bIsOutOfStamina = false;

	UPROPERTY()
	const ALadder* CurrentLadder = nullptr;

	UPROPERTY()
	const AZipline* CurrentZipline = nullptr;

	UPROPERTY()
	FHitResult CurrentWallHit;
	float DifferentSpeed(float Result) const;
	
	FMantlingMovementParameters CurrentMantlingParametrs;

	FWallRunParameters CurrentWallRunParameters;
	
	FTimerHandle MantlingTimer;
	
	float TimelineValue;
	FTimeline TimelineWallRunSideRotation;
	FRotator CharWallRunRotator = FRotator::ZeroRotator;
	FRotator StartRotation = FRotator::ZeroRotator;
	FRotator EndRotation = FRotator::ZeroRotator;
	bool bIsTestStoppedReverse = false;
	UFUNCTION()
	void UpdateRotateWallRunAnimation(float Value);

	UFUNCTION()
	void TimelineReverseWallRun();
	FTimerHandle TimerAfterEndWallRun;
	UFUNCTION()
	void EndWallRunAnimation();

	FTimeline TimelineWallRunGrowingWeight;
	UFUNCTION()
	void UpdateWallRunGrowingWeight(float Value);
	FVector CharacterGrowingWeight = FVector::ZeroVector;
	
	FRotator ForceTargetRotation = FRotator::ZeroRotator;
	bool bForceRotation = false;
	
	FTimerHandle TimerHandleToEndWallRun;
	void EndInTimeWallRun();
	void TimerToEndWallRun();

	ESide PrevWallRunSide = ESide::None;

	bool bIsSliding;
	FTimerHandle SlidingTimer;
	void TimerToEndSlide();
	bool ChangingTheAngleOfTheCharacter();
	virtual void PhysSlide(float deltaTime, int32 Iterations);
	bool bIsSlideBeginningOfTheEnd = false;
	bool bIsEndSlideChangeOnCrouch = false;
	 
	ECurrentCapsuleSize CurrentCapsuleSize = ECurrentCapsuleSize::None;

	FRotator PrevRotation = FRotator::ZeroRotator;
public:

 	
	FCapsuleSize GetCurrentCapsuleSize(const ECurrentCapsuleSize OutCurrentCapsuleSize);
	float GetCurrentWallRunSide() const;
	float GetProneCapsuleHalfHeight() const;
	bool bWantsToProne = false;
	bool bIsFullHeight = false;
	bool bProneMaintainsBaseLocation = true;
	bool bWallRunJump = false;
	FSlideParameters SlideParameters;
	bool IsOnSlide() const;
	bool GetIsSlideBeginningOfTheEnd() const;
	void SetIsSlideBeginningOfTheEnd(bool Value);
	bool GetIsEndSlideChangeOnCrouch() const;
	bool GetIsOutOfStamina() const;
};
  