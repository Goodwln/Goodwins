// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_LaunchThrowable.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UBTTaskNode_LaunchThrowable : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskNode_LaunchThrowable();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float TimeAfterLaunch = 0;
 
	
private:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	FTimerHandle IntervalAfterLaunch;

	void OnTimerAfterLaunchThrowableItem();
};
