// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "GCBasePawnAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UGCBasePawnAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
	
public:

	virtual void NativeBeginPlay() override;

	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:


	UPROPERTY(BlueprintReadWrite, Transient, Category = "BasePawnAnimInstance")
	float InputForward = 0.f;

	UPROPERTY(BlueprintReadWrite, Transient, Category = "BasePawnAnimInstance")
	float InputRight = 0.f;

	UPROPERTY(BlueprintReadWrite, Transient, Category = "BasePawnAnimInstance")
	bool bIsAir = false;

private:
	TWeakObjectPtr<class AGameCodeBasePawn> CachedBasePawn;
};
