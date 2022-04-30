// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "BTDecorator_CurrentDistance.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UBTDecorator_CurrentDistance : public UBTDecorator
{
	GENERATED_BODY()
public:
	UBTDecorator_CurrentDistance();
protected:
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	virtual void PostLoad() override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float MinDistanceThrowable = 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float MaxDistanceThrowable = 600.f;
private:
	float SquareMinDistanceThrowable = 0.f;
	float SquareMaxDistanceThrowable = 0.f;
};
