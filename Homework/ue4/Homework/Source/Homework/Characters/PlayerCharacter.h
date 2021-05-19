// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "PlayerCharacter.generated.h"

USTRUCT()
struct FIKHitTrace
{
	GENERATED_BODY()
	FHitResult OutHitLocation;
	float FootTraceOffset = 0.f;
};

UCLASS(Blueprintable)
class HOMEWORK_API APlayerCharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter(const FObjectInitializer& ObjectInitializer);

	virtual void Tick(float DeltaSeconds) override;

	virtual void MoveForward(float Value) override;
	virtual void MoveRight(float Value) override;
	virtual void Turn(float Value) override;
	virtual void LookUp(float Value) override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

	virtual bool CanJumpInternal_Implementation() const override;
	virtual void OnJumped_Implementation() override;

	FORCEINLINE float GetIKRightFootOffset() { return IKRightFootOffset; }
	FORCEINLINE float GetIKLeftFootOffset() { return IKLeftFootOffset; }
	
	FORCEINLINE float GetIKHipOffset() { return IKHipOffset; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FVector TraceBox = FVector::ZeroVector;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Camera")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|IK")
	FName LeftFootSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|IK")
	FName RightFootSocketName;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Character|IK", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKInterpSpeed = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|IK", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKTraceExtendDistance = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character|IK", meta = (ClampMin = 0.0f, UIMin = 0.0f))
	float IKHipOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|IK")
	FVector BoxExtern = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|IK")
	FRotator IKRotationBoxCollision;
private:

	FIKHitTrace GetIKOffsetForASocket(FName SocketName);

	FIKHitTrace LeftSocket;
	FIKHitTrace RightSocket;

	float IKRightFootOffset = 0.f;
	float IKLeftFootOffset = 0.f;

	float IKScale = 0.f;
	float IKTraceDistance = 0.f;

	const float MaxHipOffset = 50.0f;

	bool bIKHipOffset = false;

	bool bIsProne = false;
};