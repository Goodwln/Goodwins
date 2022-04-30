// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "Characters/BaseCharacter.h"
#include "BTService_LaunchThrowable.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UBTService_LaunchThrowable : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_LaunchThrowable();
	virtual void PostLoad() override;
	bool CanLaunchThrowable(const UBehaviorTreeComponent& OwnerComp, ABaseCharacter*& Character) const;
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float MinDistanceThrowable = 500.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float MaxDistanceThrowable = 600.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float DefaultInterval = 3.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "AI")
	float AfterLaunchInterval = 10.f;

	

private:
	float SquareMinDistanceThrowable = 0.f;
	float SquareMaxDistanceThrowable = 0.f;
};

