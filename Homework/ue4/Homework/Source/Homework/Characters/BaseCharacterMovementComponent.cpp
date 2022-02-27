// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterMovementComponent.h"
#include "BaseCharacter.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/MovementComponent.h"
#include "Components/ActorComponent.h"
#include "Actors/Interactive/Enviroment/Ladder.h"
#include "Actors/Interactive/Enviroment/Zipline.h"
#include "Curves/CurveVector.h"
#include "Homework/Components/DetectorComponents/FloorDetectorComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBaseCharacterMovementComponent::UBaseCharacterMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}
	
bool UBaseCharacterMovementComponent::IsProne() const
{
	return GetBaseCharacterOwner() && GetBaseCharacterOwner()->bIsProne;
}

float UBaseCharacterMovementComponent::GetMaxSpeed() const
{
	float Result = Super::GetMaxSpeed();

	Result = DifferentSpeed(Result);

	return Result;
}

float UBaseCharacterMovementComponent::DifferentSpeed(float Result) const
{
	if (bIsSprinting)
	{
		Result = SprintSpeed;
	}
	else if (IsSwimming())
	{
		Result = SwimmingMaxSpeed;
	}
	else if (IsCrouching())
	{
		Result = MaxWalkSpeedCrouched;
	}
	else if (IsProne())
	{
		Result = MaxProneSpeed;
	}	
	else if (bIsOutOfStamina)
	{
		Result = OutStaminaWalkSpeed;
	}
	else if(IsOnWallRun())
	{
		Result = MaxWallRunSpeed;
	}
	else if(GetBaseCharacterOwner()->IsAiming())
	{
		Result = GetBaseCharacterOwner()->GetCurrentAimingMovementSpeed();
	}
	return Result;
}

void UBaseCharacterMovementComponent::UpdateRotateWallRunAnimation(float Value)
{
	GetBaseCharacterOwner()->GetMesh()->SetRelativeRotation(FMath::Lerp(StartRotation, EndRotation, Value));
}

void UBaseCharacterMovementComponent::TimelineReverseWallRun()
{
	if(!IsOnWallRun())
	{
		bIsTestStoppedReverse = true;
		EndWallRunAnimation();
	}
}

void UBaseCharacterMovementComponent::EndWallRunAnimation()
{
	if(bIsTestStoppedReverse)
	{
		TimelineWallRunSideRotation.Reverse();
	}
}

void UBaseCharacterMovementComponent::UpdateWallRunGrowingWeight(float Value)
{
	CharacterGrowingWeight = FVector(0.f, 0.f, Value);
}

void UBaseCharacterMovementComponent::EndInTimeWallRun()
{
	WallRunEnd(EEndingWallRunMethod::Fall);
}

FCapsuleSize UBaseCharacterMovementComponent::GetCurrentCapsuleSize(const ECurrentCapsuleSize OutCurrentCapsuleSize)
{
	CurrentCapsuleSize = OutCurrentCapsuleSize;
	
	if(CapsuleSizeList.Contains(CurrentCapsuleSize))
	{
		return CapsuleSizeList[CurrentCapsuleSize];
	}

	return FCapsuleSize(0.f, 0.f);
}

float UBaseCharacterMovementComponent::GetCurrentWallRunSide() const
{
	return CurrentWallRunParameters.CurrentWallRunSide;
}

float UBaseCharacterMovementComponent::GetProneCapsuleHalfHeight() const
{
	return ProneCapsuleHalfHeight;
}

void UBaseCharacterMovementComponent::UpdateCharacterStateBeforeMovement(float DeltaSeconds)
{
	Super::UpdateCharacterStateBeforeMovement(DeltaSeconds);

	if (GetBaseCharacterOwner()->GetLocalRole() != ROLE_SimulatedProxy)
	{
		const bool bIsProning = IsProne();
		// Check for a change in Prone state. Players toggle crouch by changing bWantsToProne.
		if (bIsProning && (!bWantsToProne || !CanProneInCurrentState()))
		{
			UnProne();
		}
		else if (!bIsProning && bWantsToProne && CanProneInCurrentState())
		{
			Prone(false);
		}
	}
}																										

void UBaseCharacterMovementComponent::OnMovementModeChanged(EMovementMode PreviousMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, PreviousCustomMode);

	if (!HasValidData())
	{
		return;
	}

	if(MovementMode == MOVE_Walking)
	{
		PrevWallRunSide = ESide::None;
		
	}
	
	if (MovementMode == MOVE_Swimming)
	{
		
		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(SwimmingCapsuleRadius, SwimmingCapsuleHalfSize);
		bOrientRotationToMovement = true;
		CharacterOwner->bUseControllerRotationYaw = false;
	}
	else if (PreviousMovementMode == MOVE_Swimming)
	{
		const float DefaultCapsuleRadius = GetCurrentCapsuleSize(ECurrentCapsuleSize::DefaultCapsule).Radius;
		const float DefaultCapsuleHalfHeight = GetCurrentCapsuleSize(ECurrentCapsuleSize::DefaultCapsule).HalfHeight;

		CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCapsuleRadius, DefaultCapsuleHalfHeight, true);
		bOrientRotationToMovement = false;
		CharacterOwner->bUseControllerRotationYaw = true;
	}
	
	if(PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Ladder)
	{
		CurrentLadder = nullptr;
		CharacterOwner->bUseControllerRotationYaw = true;
	}
	
	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Slide)
	{
		bIsSlideBeginningOfTheEnd = false;
	}

	if (PreviousMovementMode == MOVE_Custom && PreviousCustomMode == (uint8)ECustomMovementMode::CMOVE_Zipline)
	{
		bIsSlideBeginningOfTheEnd = false;

		bOrientRotationToMovement = false;
		CharacterOwner->bUseControllerRotationYaw = true;
	}
	
	if (MovementMode == MOVE_Custom)
	{
		switch (CustomMovementMode)
		{
			case (uint8)ECustomMovementMode::CMOVE_Mantling:
			{
				GetWorld()->GetTimerManager().SetTimer(MantlingTimer, this, &UBaseCharacterMovementComponent::EndMantle, CurrentMantlingParametrs.Duration, false);
				break;
			}
			case (uint8)ECustomMovementMode::CMOVE_Ladder:
			{
				CharacterOwner->bUseControllerRotationYaw = false;
				break;
			}
			case(uint8)ECustomMovementMode::CMOVE_Zipline:
			{
				bOrientRotationToMovement = true;
				CharacterOwner->bUseControllerRotationYaw = false;
				break;
			}
		default:
			break;
		}
	}
}

void UBaseCharacterMovementComponent::Prone(bool bClientSimulation)
{
	if (!HasValidData())
	{
		return;
	}

	if(IsOnZipline())
	{
		return;
	}
	
	if (!CanProneInCurrentState())
	{
		return;
	}
	
	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == ProneCapsuleHalfHeight)
	{
		if (!bClientSimulation)
		{
			GetBaseCharacterOwner()->bIsProne = true;
			bWantsToCrouch = false;
			GetBaseCharacterOwner()->bIsCrouched = false;
		}

		GetBaseCharacterOwner()->StartProne(0.f, 0.f); 
		return;
	}
	
	// Change collision size to proning dimensions
	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	const float OldUnscaledRadius = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleRadius();

	// Height is not allowed to be smaller than radius.
	const float ClampedPronedHalfHeight = FMath::Max3(0.f, OldUnscaledRadius, GetCurrentCapsuleSize(ECurrentCapsuleSize::ProneCapsule).HalfHeight);
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, ClampedPronedHalfHeight);
	float HalfHeightAdjust = (OldUnscaledHalfHeight - ClampedPronedHalfHeight);
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	if (!bClientSimulation)
	{
		// Proning to a larger height? (this is rare)
		if (ClampedPronedHalfHeight > OldUnscaledHalfHeight)
		{
			FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
			FCollisionResponseParams ResponseParam;
			InitCollisionParams(CapsuleParams, ResponseParam);
			const bool bEnproned = GetWorld()->OverlapBlockingTestByChannel(UpdatedComponent->GetComponentLocation() - FVector(0.f, 0.f, ScaledHalfHeightAdjust), FQuat::Identity,
				UpdatedComponent->GetCollisionObjectType(), GetPawnCapsuleCollisionShape(SHRINK_None), CapsuleParams, ResponseParam);

			// If enproned, cancel
			if (bEnproned)
			{
				CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(OldUnscaledRadius, OldUnscaledHalfHeight);
				return;
			}
		}
			// Intentionally not using MoveUpdatedComponent, where a horizontal plane constraint would prevent the base of the capsule from staying at the same spot.
			UpdatedComponent->MoveComponent(FVector(0.f, 0.f, -ScaledHalfHeightAdjust), UpdatedComponent->GetComponentQuat(), true, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);

		GetBaseCharacterOwner()->bIsProne = true;
		bWantsToCrouch = false;
		GetBaseCharacterOwner()->bIsCrouched = false;
	}

	bForceNextFloorCheck = true;

	// OnStartProne takes the change from the Default size, not the current one (though they are usually the same).
	HalfHeightAdjust = (GetCurrentCapsuleSize(ECurrentCapsuleSize::DefaultCapsule).HalfHeight - GetCurrentCapsuleSize(ECurrentCapsuleSize::CrouchCapsule).HalfHeight - ClampedPronedHalfHeight);
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	GetBaseCharacterOwner()->StartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void UBaseCharacterMovementComponent::UnProne(bool bClientSimulation /*= false*/)
{
	if (!HasValidData())
	{
		return;			
	}

	ACharacter* DefaultCharacter = CharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	float DesiredHeight = bIsFullHeight ?
		GetCurrentCapsuleSize(ECurrentCapsuleSize::DefaultCapsule).HalfHeight : CrouchedHalfHeight;

	// See if collision is already at desired size.
	if (CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() == DesiredHeight)
	{
		if (!bClientSimulation)
		{
			GetBaseCharacterOwner()->bIsProne = false;  
			
			bWantsToCrouch = !bIsFullHeight;
			GetBaseCharacterOwner()->bIsCrouched = !bIsFullHeight;
		}
		GetBaseCharacterOwner()->EndProne(0.f, 0.f);
		return;
	}

	// const float CurrentPronedHalfHeight = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	// const float CurrentPronedRadius = CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius();

	const float ComponentScale = CharacterOwner->GetCapsuleComponent()->GetShapeScale();
	const float OldUnscaledHalfHeight = CharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float HalfHeightAdjust = DesiredHeight - OldUnscaledHalfHeight;
	float ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;
	const FVector PawnLocation = UpdatedComponent->GetComponentLocation();

	// Grow to unprone size.
	check(CharacterOwner->GetCapsuleComponent());

	if (!bClientSimulation)
	{
		// Try to stay in place and see if the larger capsule fits. We use a slightly taller capsule to avoid penetration.
		const UWorld* MyWorld = GetWorld();
		constexpr float SweepInflation = KINDA_SMALL_NUMBER * 10.f;
		FCollisionQueryParams CapsuleParams(SCENE_QUERY_STAT(ProneTrace), false, CharacterOwner);
		FCollisionResponseParams ResponseParam;
		InitCollisionParams(CapsuleParams, ResponseParam);

		// Compensate for the difference between current capsule size and standing size
		const FCollisionShape StandingCapsuleShape = GetPawnCapsuleCollisionShape(SHRINK_HeightCustom, -SweepInflation - ScaledHalfHeightAdjust); // Shrink by negative amount, so actually grow it.
		const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();
		bool bEnproned = true;

			// Expand while keeping base location the same.
			FVector StandingLocation = PawnLocation + FVector(0.f, 0.f, StandingCapsuleShape.GetCapsuleHalfHeight() - GetCurrentCapsuleSize(ECurrentCapsuleSize::ProneCapsule).HalfHeight);
			bEnproned = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);

			if (bEnproned)
			{
				if (IsMovingOnGround())
				{
					// Something might be just barely overhead, try moving down closer to the floor to avoid it.
					constexpr float MinFloorDist = KINDA_SMALL_NUMBER * 10.f;
					if (CurrentFloor.bBlockingHit && CurrentFloor.FloorDist > MinFloorDist)
					{
						StandingLocation.Z -= CurrentFloor.FloorDist - MinFloorDist;
						bEnproned = MyWorld->OverlapBlockingTestByChannel(StandingLocation, FQuat::Identity, CollisionChannel, StandingCapsuleShape, CapsuleParams, ResponseParam);
					}
				}
			}

			if (!bEnproned)
			{
				// Commit the change in location.
				UpdatedComponent->MoveComponent(StandingLocation - PawnLocation, UpdatedComponent->GetComponentQuat(), false, nullptr, EMoveComponentFlags::MOVECOMP_NoFlags, ETeleportType::TeleportPhysics);
				bForceNextFloorCheck = true;
			}

		// If still encroached then abort.
		if (bEnproned)
		{
			return;
		}

		GetBaseCharacterOwner()->bIsProne = false;
		bWantsToCrouch = !bIsFullHeight;
		GetBaseCharacterOwner()->bIsCrouched = !bIsFullHeight;
	}
	else
	{
		bShrinkProxyCapsule = true;
	}

	// Now call SetCapsuleSize() to cause touch/untouch events and actually grow the capsule
	CharacterOwner->GetCapsuleComponent()->SetCapsuleSize(DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius(), DesiredHeight, true);
	
	HalfHeightAdjust = bIsFullHeight ?
		CrouchedHalfHeight - ProneCapsuleHalfHeight : DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() - DesiredHeight - ProneCapsuleHalfHeight;
	ScaledHalfHeightAdjust = HalfHeightAdjust * ComponentScale;

	AdjustProxyCapsuleSize();
	GetBaseCharacterOwner()->EndProne(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

bool UBaseCharacterMovementComponent::CanProneInCurrentState() const
{
	return IsMovingOnGround() && UpdatedComponent && !UpdatedComponent->IsSimulatingPhysics();
}

void UBaseCharacterMovementComponent::StartSprint()
{
	bIsSprinting = true;
	bForceMaxAccel = 1;
}

void UBaseCharacterMovementComponent::StopSprint()
{
	bIsSprinting = false;	
	bForceMaxAccel = 0;
}

void UBaseCharacterMovementComponent::StartMantle(const FMantlingMovementParameters& MantlingParameters)
{
	TimelineWallRunSideRotation.Reverse();
	CurrentMantlingParametrs = MantlingParameters;
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Mantling);
}

void UBaseCharacterMovementComponent::EndMantle()
{
	SetMovementMode(MOVE_Walking);
}

bool UBaseCharacterMovementComponent::IsMantling() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Mantling;
}

void UBaseCharacterMovementComponent::SetIsOutOfStamina(bool bIsOutOfStamina_In)
{
	bIsOutOfStamina = bIsOutOfStamina_In;
}

void UBaseCharacterMovementComponent::AttachToLadder(const ALadder* Ladder)
{
	CurrentLadder = Ladder;
	
	FRotator TargetOrientationRotation = CurrentLadder->GetActorForwardVector().ToOrientationRotator();
	TargetOrientationRotation.Yaw += 180.f;

	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	const FVector LadderForwardVector = CurrentLadder->GetActorForwardVector();
	const float ActorToLadderProjection = GetActorToCurrentLadderProjection(GetActorLocation());
	
	FVector NewCharacterLocation = CurrentLadder->GetActorLocation() + ActorToLadderProjection * LadderUpVector + LadderToCharacterOffset * LadderForwardVector;

	if(CurrentLadder->GetIsOnTop())
	{
		NewCharacterLocation = CurrentLadder->GetAttachFromTopAnimMontageStartingLocation();
	}
	GetOwner()->SetActorLocation(NewCharacterLocation);
	GetOwner()->SetActorRotation(TargetOrientationRotation);

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Ladder);
																																																																																																																																																																																																											}

float UBaseCharacterMovementComponent::GetActorToCurrentLadderProjection(const FVector& Location) const
{
	checkf(IsValid(CurrentLadder), TEXT("UBaseCharacterMovementComponent::GetCharacterToCurrentLadderProjection() cannot be invoke when current ladder is null"))

	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	const FVector LadderToCharacterDistance = Location - CurrentLadder->GetActorLocation();
	return FVector::DotProduct(LadderUpVector, LadderToCharacterDistance);
}									

void UBaseCharacterMovementComponent::DetachFromLadder(EDetachFromLadderMethod DetachFromLadderMethod /*= EDetachFromLadderMethod::Fall*/)
{
	switch (DetachFromLadderMethod)
	{
	
	case EDetachFromLadderMethod::ReachingTheTop:
	{
			GetBaseCharacterOwner()->Mantle(true);
			break;
	}
	case EDetachFromLadderMethod::ReachingTheBottom:
	{
		SetMovementMode(MOVE_Walking);
		break;
	}
	case EDetachFromLadderMethod::JumpOff:
	{
		const FVector JumpDirection = CurrentLadder->GetActorForwardVector();
		SetMovementMode(MOVE_Falling);
		const FVector JumpVelocity = JumpDirection * JumpOffFromLadderSpeed;
		ForceTargetRotation = JumpDirection.ToOrientationRotator();
		bForceRotation = true;
		Launch(JumpVelocity);
		break;
	}
	case EDetachFromLadderMethod::Fall:
		SetMovementMode(MOVE_Falling);
	default:
		break;
	}
}

bool UBaseCharacterMovementComponent::IsOnLadder() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Ladder;
}

const class ALadder* UBaseCharacterMovementComponent::GetCurrentLadder()
{	
	return CurrentLadder; 
}

void UBaseCharacterMovementComponent::PhysCustom(float deltaTime, int32 Iterations)
{
	switch (CustomMovementMode)
	{
	case (uint8)ECustomMovementMode::CMOVE_Zipline:
		{
			PhysZipline(deltaTime, Iterations);
			break;
		}
	case (uint8)ECustomMovementMode::CMOVE_Ladder:
		{
			PhysLadder(deltaTime, Iterations);
			break;
		}
	case (uint8)ECustomMovementMode::CMOVE_Mantling:
		{
			PhysMantling(deltaTime, Iterations);
			break;
		}
	case (uint8)ECustomMovementMode::CMOVE_WallRun:
		{
			PhysWallRun(deltaTime, Iterations);
			break;
		}
	case (uint8)ECustomMovementMode::CMOVE_Slide:
		{
			PhysSlide(deltaTime, Iterations);
			break;
		}
	default:
		break;
	}

	Super::PhysCustom(deltaTime, Iterations);
}

void UBaseCharacterMovementComponent::PhysMantling(float DeltaTime, int32 Iterations)
{
	const float ElapsedTime = GetWorld()->GetTimerManager().GetTimerElapsed(MantlingTimer) + CurrentMantlingParametrs.StartTime;

	const FVector MantlingCurveValue = CurrentMantlingParametrs.MantlingCurve->GetVectorValue(ElapsedTime);

	const float PositionAlpha = MantlingCurveValue.X;
	const float XYCorrectionAlpha = MantlingCurveValue.Y;
	const float ZCorrectionAlpha = MantlingCurveValue.Z;

	FVector CorrectedInitialLocation = FMath::Lerp(CurrentMantlingParametrs.InitialLocation, CurrentMantlingParametrs.InitialAnimationLocation, XYCorrectionAlpha);
	CorrectedInitialLocation.Z = FMath::Lerp(CurrentMantlingParametrs.InitialLocation.Z, CurrentMantlingParametrs.InitialAnimationLocation.Z, ZCorrectionAlpha);

	if (IsValid(CurrentMantlingParametrs.ActorTargetLedge))
	{                                       
		if (CurrentMantlingParametrs.SaveLocation != CurrentMantlingParametrs.ActorTargetLedge->GetComponentLocation())
		{
			CurrentMantlingParametrs.SaveLocation = CurrentMantlingParametrs.ActorTargetLedge->GetComponentLocation();
			
			CurrentMantlingParametrs.TargetLocation = CurrentMantlingParametrs.SaveLocation + CurrentMantlingParametrs.MantlingOffset;
		}
	}
	MoveUpdateMantling(CorrectedInitialLocation,CurrentMantlingParametrs.TargetLocation, PositionAlpha);
}

void UBaseCharacterMovementComponent::MoveUpdateMantling(FVector CorrectedInitialLocation, FVector &TargetUpdateLocation, float PositionAlpha)
{
	const FVector NewLocation = FMath::Lerp(CorrectedInitialLocation, TargetUpdateLocation, PositionAlpha);
	const FRotator NewRotation = FMath::Lerp(CurrentMantlingParametrs.InitialRotation, CurrentMantlingParametrs.TargetRotation, PositionAlpha);

	const FVector Delta = NewLocation - GetActorLocation();

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, NewRotation, false, Hit);
}

void UBaseCharacterMovementComponent::PhysicsRotation(float DeltaTime)
{
	if(bForceRotation)
	{
		FRotator CurrentRotation = UpdatedComponent->GetComponentRotation(); // Normalized
		CurrentRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): CurrentRotation"));

		FRotator DeltaRot = GetDeltaRotation(DeltaTime);
		DeltaRot.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): GetDeltaRotation"));

		// Accumulate a desired new rotation.
		const float AngleTolerance = 1e-3f;

		if (!CurrentRotation.Equals(ForceTargetRotation, AngleTolerance))
		{
			FRotator DesiredRotation = ForceTargetRotation;
			// PITCH
			if (!FMath::IsNearlyEqual(CurrentRotation.Pitch, DesiredRotation.Pitch, AngleTolerance))
			{
				DesiredRotation.Pitch = FMath::FixedTurn(CurrentRotation.Pitch, DesiredRotation.Pitch, DeltaRot.Pitch);
			}

			// YAW
			if (!FMath::IsNearlyEqual(CurrentRotation.Yaw, DesiredRotation.Yaw, AngleTolerance))
			{
				DesiredRotation.Yaw = FMath::FixedTurn(CurrentRotation.Yaw, DesiredRotation.Yaw, DeltaRot.Yaw);
			}

			// ROLL
			if (!FMath::IsNearlyEqual(CurrentRotation.Roll, DesiredRotation.Roll, AngleTolerance))
			{
				DesiredRotation.Roll = FMath::FixedTurn(CurrentRotation.Roll, DesiredRotation.Roll, DeltaRot.Roll);
			}

			// Set the new rotation.
			DesiredRotation.DiagnosticCheckNaN(TEXT("CharacterMovementComponent::PhysicsRotation(): DesiredRotation"));
			MoveUpdatedComponent( FVector::ZeroVector, DesiredRotation, /*bSweep*/ false );
		}
		else
		{
			ForceTargetRotation = FRotator::ZeroRotator;
			bForceRotation = false;
		}
		return;
	}
	
	if (IsOnLadder())
	{
		return;
	}
	Super::PhysicsRotation(DeltaTime);
}

void UBaseCharacterMovementComponent::InitTimelineWallRunSideRotation()
{
	if(RotateWallRun)
	{
		FOnTimelineFloat TimelineCallback;
		FOnTimelineEventStatic TimelineFinishedCallback;

		TimelineCallback.BindUFunction(this, FName("UpdateRotateWallRunAnimation"));
		TimelineFinishedCallback.BindUFunction(this, FName("EndWallRunAnimation"));
		TimelineWallRunSideRotation.AddInterpFloat(RotateWallRun, TimelineCallback);
		TimelineWallRunSideRotation.SetTimelineFinishedFunc(TimelineFinishedCallback);
		TimelineWallRunSideRotation.SetPlayRate(1.f);
		StartRotation = GetBaseCharacterOwner()->GetMesh()->GetRelativeRotation();
	}
}

void UBaseCharacterMovementComponent::InitTimelineWallRunGrowingWeight()
{
	if(GrowingWeight && bIsVerticalChangeWallRun)
	{
		FOnTimelineFloat TimelineCallback;
		
		TimelineCallback.BindUFunction(this, FName("UpdateWallRunGrowingWeight"));
		TimelineWallRunGrowingWeight.AddInterpFloat(GrowingWeight, TimelineCallback);
		TimelineWallRunGrowingWeight.SetPlayRate(1.f);
	}
}

void UBaseCharacterMovementComponent::TimerToEndWallRun()
{
	if(bIsEndlessWallRun)
	{
		return;
	}
	GetWorld()->GetTimerManager().SetTimer(TimerHandleToEndWallRun, this, &UBaseCharacterMovementComponent::EndInTimeWallRun, MaxWallRunTime, false);
}

void UBaseCharacterMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	InitTimelineWallRunSideRotation();
	InitTimelineWallRunGrowingWeight();
	
}

void UBaseCharacterMovementComponent::TickComponent(float DeltaTime, ELevelTick Tick,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, Tick, ThisTickFunction);

	TimelineWallRunSideRotation.TickTimeline(DeltaTime);
	TimelineWallRunGrowingWeight.TickTimeline(DeltaTime);
}

void UBaseCharacterMovementComponent::PhysLadder(float deltaTime, int32 Iterations)
{
	CalcVelocity(deltaTime, 1.f, false, ClimbingOnLadderBrakingDeceleration);
	const FVector Delta = Velocity * deltaTime;
	
	if(HasAnimRootMotion())
	{
		FHitResult Hit;
		SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), false, Hit);
		return;
	}
	
	const FVector NewPos = GetActorLocation() + Delta;
	const float NewPosProjection = GetActorToCurrentLadderProjection(NewPos);

	if (NewPosProjection < MinLadderBottomOffset)
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheBottom);
		return;												
	}
	else if (NewPosProjection > (CurrentLadder->GetLadderHeight() - MaxLadderTopOffset))
	{
		DetachFromLadder(EDetachFromLadderMethod::ReachingTheTop);
		
		return;
	}

	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
}

float UBaseCharacterMovementComponent::GetLadderSpeedRatio() const
{
	checkf(IsValid(CurrentLadder), TEXT("UBaseCharacterMovementComponent::GetCharacterToCurrentLadderProjection() cannot be invoke when current ladder is null"));
	
	const FVector LadderUpVector = CurrentLadder->GetActorUpVector();
	return FVector::DotProduct(LadderUpVector, Velocity) / MaxClimbingOnLadderSpeed;
}

FVector UBaseCharacterMovementComponent::GetLocationCharacterToZipline()
{
	const FVector CableLocation = CurrentZipline->GetCable()->GetComponentLocation();
	const float CapsuleHalfHeight = GetBaseCharacterOwner()->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	
	float PositionActorFromCenter = FVector(CableLocation - GetActorLocation()).Size();
	
	FVector CharacterUnderCable((CableLocation));
	CharacterUnderCable.Z -= CapsuleHalfHeight + CharacterUnderZiplineOffset;
	
	const float ArcCosAngle = FVector::DotProduct((GetActorLocation() - CableLocation).GetSafeNormal(), CurrentZipline->GetDirectionCableStatic().Vector());
	const float Angle = UKismetMathLibrary::DegAcos(ArcCosAngle);

	PositionActorFromCenter = 90.f > Angle ? PositionActorFromCenter : -PositionActorFromCenter;

	return CharacterUnderCable + CurrentZipline->GetCable()->GetForwardVector() * PositionActorFromCenter;
}

void UBaseCharacterMovementComponent::AttachToZipline(const AZipline* Zipline)
{
	CurrentZipline = Zipline;

	PrevRotation = GetBaseCharacterOwner()->GetCapsuleComponent()->GetComponentRotation();
	GetBaseCharacterOwner()->SetActorRotation(CurrentZipline->GetDirectionCableStatic());
	GetBaseCharacterOwner()->SetActorLocation(GetLocationCharacterToZipline());
	
	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Zipline);
}

void UBaseCharacterMovementComponent::DetachFromZipline()
{
	GetBaseCharacterOwner()->SetActorRotation(PrevRotation);
	SetMovementMode(MOVE_Falling);
}

bool UBaseCharacterMovementComponent::IsOnZipline() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Zipline;
}

void UBaseCharacterMovementComponent::PhysZipline(float DeltaTime, int32 Iterations)
{	
	CalcVelocity(DeltaTime, 1.f, false, ClimbingOnLadderBrakingDeceleration);
	
	const FVector Delta = (GetBaseCharacterOwner()->GetActorForwardVector() * CharacterSpeedToZipline) * DeltaTime;
	
	const float DistanceA = CurrentZipline->GetCableLength();
	const float DistanceB = (GetActorLocation() - CurrentZipline->GetHighPillar()->GetComponentLocation()).Size();
	
	if((DistanceA - CurrentZipline->GetLowPillar()->GetStaticMesh()->GetBoundingBox().GetSize().X) <= DistanceB)
	{
		DetachFromZipline();
		return;
	}
	
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);

	if(Hit.bBlockingHit)
	{
		DetachFromZipline();
	}
}

void UBaseCharacterMovementComponent::WallRunStart(FWallRunParameters& WallRunParameters)
{
	bIsTestStoppedReverse = false;
	
	CurrentWallRunParameters = WallRunParameters;
	if(PrevWallRunSide == CurrentWallRunParameters.Side)
	{
		return;
	}
	
	FRotator CharacterRotation = GetBaseCharacterOwner()->GetMesh()->GetRelativeRotation();

	float RotateMesh = CurrentWallRunParameters.Side == ESide::Left ? -(90.f - CurrentWallRunParameters.Angle) : 90.f - CurrentWallRunParameters.Angle;
	
	TimelineWallRunGrowingWeight.PlayFromStart();

	 if(StartRotation.Pitch == CharacterRotation.Pitch)
	 {
	 	CharacterRotation.Pitch += RotateMesh;
		EndRotation = CharacterRotation;
	 	TimelineWallRunSideRotation.Play();
	 }
	else
	{
		EndRotation = CharacterRotation;
		TimelineWallRunSideRotation.Play();
	}
	TimerToEndWallRun();
	SetMovementMode(EMovementMode::MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_WallRun);
}

void UBaseCharacterMovementComponent::LaunchJumpDirection(FVector JumpDirection)
{
	FVector JumpVelocity = JumpDirection * JumpOffFromWallSpeed;
	ForceTargetRotation = JumpDirection.ToOrientationRotator();
	bForceRotation = true;
	Launch(JumpVelocity);
}

void UBaseCharacterMovementComponent::WallRunEnd(EEndingWallRunMethod EndingWallRun)
{
	if(IsMantling() || IsOnZipline() || IsProne() || IsOnLadder() || IsSprinting() || IsMovingOnGround())
	{
		return;
	}
	
	PrevWallRunSide = CurrentWallRunParameters.Side;
	GetWorld()->GetTimerManager().SetTimer(TimerAfterEndWallRun, this, &UBaseCharacterMovementComponent::TimelineReverseWallRun, 0.2f, false, -1.f);

	switch (EndingWallRun)
	{
	case EEndingWallRunMethod::JumpOff:
		{
			SetMovementMode(MOVE_Falling);
			
			if(CurrentWallRunParameters.Side == ESide::Left)
			{
				const FVector JumpDirection = GetBaseCharacterOwner()->GetActorForwardVector() + GetBaseCharacterOwner()->GetActorRightVector();
				LaunchJumpDirection(JumpDirection);
			}
			else
			{
				const FVector JumpDirection = GetBaseCharacterOwner()->GetActorForwardVector() + -GetBaseCharacterOwner()->GetActorRightVector();
				LaunchJumpDirection(JumpDirection);
			}
			break;
		}
	case EEndingWallRunMethod::Fall:
		default:
		{
			SetMovementMode(MOVE_Falling);
			break;
		}
	}
}

bool UBaseCharacterMovementComponent::IsOnWallRun() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_WallRun;
}

void UBaseCharacterMovementComponent::PhysWallRun(float DeltaTime, int32 Iterations)
{
	CalcVelocity(DeltaTime, 1.f, false, ClimbingOnLadderBrakingDeceleration);

	const FVector Delta = (CharacterGrowingWeight + Velocity) * DeltaTime;

	if(bWallRunJump)
	{
		bWallRunJump = false;
		WallRunEnd(EEndingWallRunMethod::JumpOff);
	}
	
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta, GetOwner()->GetActorRotation(), true, Hit);
	SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, false);
}

void UBaseCharacterMovementComponent::StartSlide(FSlideParameters& OutSlideParameters)
{
	SlideParameters = OutSlideParameters;
	bIsSliding = true;
	SetIsSlideBeginningOfTheEnd(false);
	
	GetBaseCharacterOwner()->GetCapsuleComponent()->SetCapsuleHalfHeight(GetCurrentCapsuleSize(ECurrentCapsuleSize::SlideCapsule).HalfHeight);
	GetBaseCharacterOwner()->GetCapsuleComponent()->SetCapsuleRadius(GetCurrentCapsuleSize(ECurrentCapsuleSize::SlideCapsule).Radius);
	
	const float CapsuleLocationOffset = GetCurrentCapsuleSize(ECurrentCapsuleSize::DefaultCapsule).HalfHeight - GetCurrentCapsuleSize(ECurrentCapsuleSize::SlideCapsule).HalfHeight;
	FVector NewCapsuleLocation = GetBaseCharacterOwner()->GetActorLocation();
	NewCapsuleLocation.Z -= CapsuleLocationOffset;
	
	GetBaseCharacterOwner()->SetActorLocation(NewCapsuleLocation);
	
	if(SlideParameters.Angle == 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(SlidingTimer, this, &UBaseCharacterMovementComponent::TimerToEndSlide, SlideMaxTime, false, SlideMaxTime);
	}

	SetMovementMode(MOVE_Custom, (uint8)ECustomMovementMode::CMOVE_Slide);
	FRotator NewRotation = GetBaseCharacterOwner()->GetCapsuleComponent()->GetComponentRotation();
	NewRotation.Pitch -= SlideParameters.Angle;
	GetBaseCharacterOwner()->GetCapsuleComponent()->SetWorldRotation(NewRotation);
}

void UBaseCharacterMovementComponent::StopSlide()
{

	GetWorld()->GetTimerManager().ClearTimer(SlidingTimer);
	bIsSliding = false;
	SetIsSlideBeginningOfTheEnd(false);
	
	GetBaseCharacterOwner()->GetCapsuleComponent()->SetCapsuleHalfHeight(GetCurrentCapsuleSize(ECurrentCapsuleSize::DefaultCapsule).HalfHeight);
	GetBaseCharacterOwner()->GetCapsuleComponent()->SetCapsuleRadius(GetCurrentCapsuleSize(ECurrentCapsuleSize::DefaultCapsule).Radius);
	
	if(bIsEndSlideChangeOnCrouch)
	{
		SetMovementMode(MOVE_Walking);
		Crouch();
		return;
	}
	float CapsuleLocationOffset = 0;
	CapsuleLocationOffset = GetCurrentCapsuleSize(ECurrentCapsuleSize::DefaultCapsule).HalfHeight - GetCurrentCapsuleSize(ECurrentCapsuleSize::SlideCapsule).HalfHeight;
	FVector NewCapsuleLocation = GetBaseCharacterOwner()->GetActorLocation();
	NewCapsuleLocation.Z += CapsuleLocationOffset;
	
	GetBaseCharacterOwner()->SetActorLocation(NewCapsuleLocation);
	SetMovementMode(MOVE_Walking);
}

void UBaseCharacterMovementComponent::TimerToEndSlide()
{
	if(!bIsSliding)
		return;
	
	SetIsSlideBeginningOfTheEnd(true);
}

bool UBaseCharacterMovementComponent::ChangingTheAngleOfTheCharacter()
{
	FFloorProperty FloorProperty;
	if(!GetBaseCharacterOwner()->GetFloorDetectorComponent()->DetectFloor(FloorProperty))
	{
		return false;
	}
	
	if(SlideParameters.Angle != FloorProperty.Angle)
	{
		FRotator NewRotation = GetBaseCharacterOwner()->GetCapsuleComponent()->GetComponentRotation();
		NewRotation.Pitch += (SlideParameters.Angle - FloorProperty.Angle);
		SlideParameters.Angle = FloorProperty.Angle;
		
		GetBaseCharacterOwner()->GetCapsuleComponent()->SetWorldRotation(NewRotation);
		
		if(SlideParameters.Angle == 0.f)
		{
			GetWorld()->GetTimerManager().SetTimer(SlidingTimer, this, &UBaseCharacterMovementComponent::TimerToEndSlide, SlideMaxTime, false, SlideMaxTime);
		}
	
		if(SlideParameters.Angle != 0.f)
		{
			GetWorld()->GetTimerManager().ClearTimer(SlidingTimer);
		}
	}
	return true;
}

void UBaseCharacterMovementComponent::PhysSlide(float deltaTime, int32 Ite1rations)
{
	if(GetBaseCharacterOwner()->GetFloorDetectorComponent()->DetectForwardBarrier())
	{
		GetBaseCharacterOwner()->OnEndSlide();
		return;
	}

	if (!ChangingTheAngleOfTheCharacter())
	{
		GetBaseCharacterOwner()->OnEndSlide();
		return;
	}
	
	CalcVelocity(deltaTime,10.f, false, ClimbingOnLadderBrakingDeceleration);

	FVector Delta = SlideSpeed  *  (-GetBaseCharacterOwner()->GetActorUpVector() + GetBaseCharacterOwner()->GetActorForwardVector()) * deltaTime;
	
	bIsEndSlideChangeOnCrouch = GetBaseCharacterOwner()->GetFloorDetectorComponent()->DetectСeiling();
	
	FHitResult Hit;
	SafeMoveUpdatedComponent(Delta,GetOwner()->GetActorRotation(), true, Hit);

	SlideAlongSurface(Delta,1.f - Hit.Time, Hit.ImpactNormal, Hit, false);
}

bool UBaseCharacterMovementComponent::IsOnSlide() const
{
	return UpdatedComponent && MovementMode == MOVE_Custom && CustomMovementMode == (uint8)ECustomMovementMode::CMOVE_Slide;
}

bool UBaseCharacterMovementComponent::GetIsSlideBeginningOfTheEnd() const
{
	return bIsSlideBeginningOfTheEnd;
}

void UBaseCharacterMovementComponent::SetIsSlideBeginningOfTheEnd(bool Value)
{
	bIsSlideBeginningOfTheEnd = Value;
}

bool UBaseCharacterMovementComponent::GetIsEndSlideChangeOnCrouch() const
{
	return  bIsEndSlideChangeOnCrouch;
}

bool UBaseCharacterMovementComponent::GetIsOutOfStamina() const
{
	 return bIsOutOfStamina;
}

ABaseCharacter* UBaseCharacterMovementComponent::GetBaseCharacterOwner() const
{
	return StaticCast<ABaseCharacter*>(CharacterOwner);
}
