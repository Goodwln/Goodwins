// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Components/CapsuleComponent.h"
#include "BaseCharacterMovementComponent.h"
#include "Controllers/BaseCharacterPlayerController.h"
#include "Homework/Components/DetectorComponents/FloorDetectorComponent.h"

APlayerCharacter::APlayerCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->bUsePawnControlRotation = false;

	GetCharacterMovement()->bOrientRotationToMovement = 1;
	GetCharacterMovement()->RotationRate = FRotator(0.f, 540.f, 0.f);
	//GetCharacterMovement()->MaxStepHeight = 1000.f;
	
	IKScale = GetActorScale3D().Z;
	IKTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * IKScale;
}

void APlayerCharacter::TimelineFOVInitialize()
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if(IsValid(CurrentRangeWeapon) && bIsWeapon)
	{
		if(IsValid(CurrentRangeWeapon->GetFOVCurve()))
		{
			FOnTimelineFloat UpdateFunctionTimeline;
			UpdateFunctionTimeline.BindUFunction(this, TEXT("ChangeFOV"));
			FOVTimeline.AddInterpFloat(CurrentRangeWeapon->GetFOVCurve(), UpdateFunctionTimeline);
		}
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	APlayerController* PlayerController = GetController<ABaseCharacterPlayerController>();
	if(IsValid(PlayerController))
	{
		APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;
		PlayerCameraManager->SetFOV(DefaultFOV);
	}
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	TimelineFOVInitialize();
	
	FOVTimeline.TickTimeline(DeltaSeconds);
	
	LeftSocket = GetIKOffsetForASocket(LeftFootSocketName);
	RightSocket = GetIKOffsetForASocket(RightFootSocketName);

	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, RightSocket.FootTraceOffset, DeltaSeconds, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, LeftSocket.FootTraceOffset, DeltaSeconds, IKInterpSpeed);
	
	bIKHipOffset = FMath::Abs((LeftSocket.OutHitLocation.Location - RightSocket.OutHitLocation.Location).Z) < MaxHipOffset;
	IKHipOffset = FMath::FInterpTo(IKHipOffset, bIKHipOffset ? FMath::Abs((LeftSocket.OutHitLocation.Location - RightSocket.OutHitLocation.Location).Z) * -0.7f : 0.f, DeltaSeconds, IKInterpSpeed); //It is working
}

void APlayerCharacter::WallRun(float Value)
{
	FWallDescription WallDescription;
	WallDetectorComponent->GetWallDescription(WallDescription);

	FWallRunParameters WallRunParameters;
	WallRunParameters.Side = WallDescription.WallSide;
	WallRunParameters.Angle = WallDescription.AngleRotation;
	WallRunParameters.CurrentWallRunSide = WallDescription.CurrentWallSide;
	if(WallDetectorComponent->DetectWall() && !FMath::IsNearlyZero(Value, 1e-6f) && !GetBaseCharacterMovementComponent()->IsOnWallRun() && GetBaseCharacterMovementComponent()->IsFalling())
	{
		GetBaseCharacterMovementComponent()->WallRunStart(WallRunParameters);
	}

	if((!WallDetectorComponent->DetectWall() || FMath::IsNearlyZero(Value, 1e-6f)) && GetBaseCharacterMovementComponent()->IsOnWallRun())
	{
		GetBaseCharacterMovementComponent()->WallRunEnd(EEndingWallRunMethod::Fall);
	}
	
	if ( GetBaseCharacterMovementComponent()->IsOnWallRun() && !FMath::IsNearlyZero(Value, 1e-6f))
	{	
		FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);

		AddMovementInput(ForwardVector, Value);
	}
}
	
void APlayerCharacter::MoveForward(float Value)
{
	
	if(GetBaseCharacterMovementComponent()->IsOnZipline() || GetBaseCharacterMovementComponent()->IsOnLadder() || GetBaseCharacterMovementComponent()->IsMantling() || GetBaseCharacterMovementComponent()->IsOnSlide() || GetBaseCharacterMovementComponent()->IsOnWallRun() )
	{
		return;
	}
	
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{	
		FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);

		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if(GetBaseCharacterMovementComponent()->IsOnZipline() || GetBaseCharacterMovementComponent()->IsOnLadder() || GetBaseCharacterMovementComponent()->IsMantling() || GetBaseCharacterMovementComponent()->IsOnSlide() || GetBaseCharacterMovementComponent()->IsOnWallRun() )
	{
		return;
	}
	
	if ((GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling()) && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);
		
		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::SwimForward(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator PitchYawRotator(GetControlRotation().Pitch, GetControlRotation().Yaw, 0.f);
		FVector ForwardVector = PitchYawRotator.RotateVector(FVector::ForwardVector);

		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::SwimRight(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);

		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::SwimUp(float Value)
{
	if (GetCharacterMovement()->IsSwimming() && !FMath::IsNearlyZero(Value, 1e-6f))
	{
		AddMovementInput(FVector::UpVector, Value);
	}
}



void APlayerCharacter::Turn(float Value)
{
	if (IsAiming() && GetCharacterEquipmentComponent()->GetCurrentRangeWeapon())
	{
		AddControllerYawInput(Value * GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()->GetAimTurnModifier());
	}
	else
	{
		AddControllerYawInput(Value);
	}
}

void APlayerCharacter::LookUp(float Value)
{
	if(IsAiming() && GetCharacterEquipmentComponent()->GetCurrentRangeWeapon())
	{
		AddControllerPitchInput(Value * GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()->GetAimLookUpModifier());
	}
	else
	{
		AddControllerPitchInput(Value);
	}
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	IKTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * IKScale;
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);

	IKTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * IKScale;
}

void APlayerCharacter::StartProne(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{	
	Super::StartProne(HalfHeightAdjust, ScaledHalfHeightAdjust);

	IKTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * IKScale;
}

void APlayerCharacter::EndProne(float HeightAdjust, float ScaledHeightAdjust)
{
	Super::EndProne(HeightAdjust, ScaledHeightAdjust);

	IKTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * IKScale;
}

FIKHitTrace APlayerCharacter::GetIKOffsetForASocket(FName SocketName)
{
	float Result = 0.f;

	FVector SocketLocation = this->GetMesh()->GetSocketLocation(SocketName);
	FVector TraceStart(SocketLocation.X, SocketLocation.Y, GetActorLocation().Z);

	FVector TraceEnd = TraceStart - IKTraceDistance * FVector::UpVector;

	FIKHitTrace HitResult;
	ETraceTypeQuery TraceType = UEngineTypes::ConvertToTraceType(ECC_Visibility);

	if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), TraceStart, TraceEnd, BoxExtern, GetMesh()->GetSocketRotation(SocketName) + FRotator(0, 90, 90), TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult.OutHitLocation, true))
	{
		HitResult.FootTraceOffset = (HitResult.OutHitLocation.Location - GetMesh()->GetComponentLocation()).Z - IKHipOffset;
	}
	else if (UKismetSystemLibrary::BoxTraceSingle(GetWorld(), TraceStart, TraceEnd - IKTraceExtendDistance * FVector::UpVector, BoxExtern, GetMesh()->GetSocketRotation(SocketName) + FRotator(0, 90, 90), TraceType, true, TArray<AActor*>(), EDrawDebugTrace::ForOneFrame, HitResult.OutHitLocation, true))
	{
		HitResult.FootTraceOffset = (HitResult.OutHitLocation.Location - GetMesh()->GetComponentLocation()).Z - IKHipOffset;
	}

	return HitResult;
}
	
void APlayerCharacter::OnJumped_Implementation()
{
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void APlayerCharacter::ChangeFOV(float Value)
{
	APlayerController* PlayerController = GetController<ABaseCharacterPlayerController>();
	if(!IsValid(PlayerController))
	{
		return;
	}

	APlayerCameraManager* PlayerCameraManager = PlayerController->PlayerCameraManager;
	if(IsValid(PlayerCameraManager))
	{
		ARangeWeaponItem* RangeWeaponItem = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
		if(IsValid(RangeWeaponItem))
		{
			FOVValue = FMath::Lerp(DefaultFOV,RangeWeaponItem->GetAimFOV() ,Value);
			PlayerCameraManager->SetFOV(FOVValue);
		}
	}
}
                      
void APlayerCharacter::OnStartAimingInternal()
{
	Super::OnStartAimingInternal();

	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if(IsValid(CurrentRangeWeapon))
	{
		bIsWeapon = false;
	}
	
	
	FOVTimeline.Play();
}

void APlayerCharacter::OnStopAimingInternal()
{
	Super::OnStopAimingInternal();
	
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if(IsValid(CurrentRangeWeapon))
	{
		bIsWeapon = true;
	}
	
	FOVTimeline.Reverse();


}



bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	bool Result = Super::CanJumpInternal_Implementation();
	Result &= !bIsCrouched; // Result = Result || bIsCrouched;
	Result &= !GetBaseCharacterMovementComponent()->GetIsOutOfStamina();
	Result &= !GetBaseCharacterMovementComponent()->IsProne();
	return Result;
}
