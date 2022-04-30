// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_SearchTarget.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UBTService_SearchTarget : public UBTService
{
	GENERATED_BODY()
public:
	UBTService_SearchTarget();
	
protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	// FBlackboardKeySelector TargetKey;
};
