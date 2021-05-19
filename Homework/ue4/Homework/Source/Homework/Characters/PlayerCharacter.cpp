// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerCharacter.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Components/CapsuleComponent.h"
#include "BaseCharacterMovementComponent.h"


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

	IKScale = GetActorScale3D().Z;
	IKTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * IKScale;
}

void APlayerCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	LeftSocket = GetIKOffsetForASocket(LeftFootSocketName);
	RightSocket = GetIKOffsetForASocket(RightFootSocketName);

	IKRightFootOffset = FMath::FInterpTo(IKRightFootOffset, RightSocket.FootTraceOffset, DeltaSeconds, IKInterpSpeed);
	IKLeftFootOffset = FMath::FInterpTo(IKLeftFootOffset, LeftSocket.FootTraceOffset, DeltaSeconds, IKInterpSpeed);
	
	//IKHipOffset = FMath::FInterpTo(IKHipOffset, IKRightFootOffset > IKLeftFootOffset ? -IKRightFootOffset : -IKLeftFootOffset, DeltaSeconds, IKInterpSpeed); //It is working
	
	bIKHipOffset = FMath::Abs((LeftSocket.OutHitLocation.Location - RightSocket.OutHitLocation.Location).Z) < MaxHipOffset;
	IKHipOffset = FMath::FInterpTo(IKHipOffset, bIKHipOffset ? FMath::Abs((LeftSocket.OutHitLocation.Location - RightSocket.OutHitLocation.Location).Z) * -0.7f : 0.f, DeltaSeconds, IKInterpSpeed); //It is working
}
	
void APlayerCharacter::MoveForward(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		FVector ForwardVector = YawRotator.RotateVector(FVector::ForwardVector);

		AddMovementInput(ForwardVector, Value);
	}
}

void APlayerCharacter::MoveRight(float Value)
{
	if (!FMath::IsNearlyZero(Value, 1e-6f))
	{
		FRotator YawRotator(0.f, GetControlRotation().Yaw, 0.f);
		FVector RightVector = YawRotator.RotateVector(FVector::RightVector);

		AddMovementInput(RightVector, Value);
	}
}

void APlayerCharacter::Turn(float Value)
{
	AddControllerYawInput(Value);
}

void APlayerCharacter::LookUp(float Value)
{
	AddControllerPitchInput(Value);
}

void APlayerCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset += FVector(0.f, 0.f, HalfHeightAdjust);

	IKTraceDistance = GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * IKScale	;
}

void APlayerCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	SpringArmComponent->TargetOffset -= FVector(0.f, 0.f, HalfHeightAdjust);

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

bool APlayerCharacter::CanJumpInternal_Implementation() const
{
	bool Result = Super::CanJumpInternal_Implementation();

	Result |= bIsCrouched; // Result = Result || bIsCrouched;
	Result &= !BaseCharacterMovementComponent->GetIsOutOfStamina();

	return Result;
}