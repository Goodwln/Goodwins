// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTaskNode_NextPatrollingPoint.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UBTTaskNode_NextPatrollingPoint : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTaskNode_NextPatrollingPoint();

protected:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	
};
