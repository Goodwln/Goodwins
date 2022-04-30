// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_RandomPointAroundTarget.generated.h"
/**
 * 
 */

USTRUCT()
struct FRandomPointAroundTarget
{
	GENERATED_BODY()

	FVector Location;
	bool bIsFound;
};

class UNavigationSystemV1;
UCLASS()
class HOMEWORK_API UBTTask_RandomPointAroundTarget : public UBTTaskNode
{
	GENERATED_BODY()
public:
	UBTTask_RandomPointAroundTarget();

	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:

	virtual void PostLoad() override;
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float Radius = 500;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float MinDistanceToTarget = 500;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FBlackboardKeySelector TargetKey;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	FBlackboardKeySelector LocationKey;

private:
	FRandomPointAroundTarget RandomPointAroundTarget(UNavigationSystemV1* NavSys, AActor* TargetActor);

	float SquareMinDistanceToTarget = 0;
};
