// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GCBasePawnMovementComponent.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UGCBasePawnMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:

	void JumpStart();

	virtual bool IsFalling() const override;
protected:

	UPROPERTY(EditAnywhere)
	float MaxSpeed = 1200.f;

	UPROPERTY(EditAnywhere)
	bool bGravityEnable = true;

	UPROPERTY(EditAnywhere)
	float InititalJumpVelocity = 500.f;

private:
	FVector VerticalVelocity = FVector::ZeroVector;
	bool bIsFalling = false;
};
