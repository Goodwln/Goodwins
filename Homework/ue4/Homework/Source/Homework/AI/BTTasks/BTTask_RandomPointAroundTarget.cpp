// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_RandomPointAroundTarget.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"

UBTTask_RandomPointAroundTarget::UBTTask_RandomPointAroundTarget()
{
	NodeName = "Random point around target";
}

EBTNodeResult::Type UBTTask_RandomPointAroundTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();

	if(!IsValid(AIController) || !IsValid(Blackboard))
	{
		return EBTNodeResult::Failed;
	}

	APawn* Pawn = AIController->GetPawn();
	if(!IsValid(Pawn))
	{
		return EBTNodeResult::Failed;
	}
	
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn);
	if(!IsValid(NavSys))
	{
		return EBTNodeResult::Failed;
	}

	AActor* TargetActor = Cast<AActor>(Blackboard->GetValueAsObject(TargetKey.SelectedKeyName));
	if(!IsValid(TargetActor))
	{
		return EBTNodeResult::Failed;
	}

	const FRandomPointAroundTarget Result = RandomPointAroundTarget(NavSys, TargetActor);
	if(!Result.bIsFound)
	{
		return EBTNodeResult::Failed;
	}
 
	Blackboard->SetValueAsVector(LocationKey.SelectedKeyName, Result.Location);
	return EBTNodeResult::Succeeded;
}

void UBTTask_RandomPointAroundTarget::PostLoad()
{
	Super::PostLoad();
	
	SquareMinDistanceToTarget = MinDistanceToTarget * MinDistanceToTarget;
}

#if WITH_EDITOR
void UBTTask_RandomPointAroundTarget::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if(MinDistanceToTarget > Radius)
	{
		MinDistanceToTarget = Radius;	
	}
}
#endif

FRandomPointAroundTarget UBTTask_RandomPointAroundTarget::RandomPointAroundTarget(UNavigationSystemV1* NavSys, AActor* TargetActor)
{
	// TODO Ограничить количество рекурсивных вызовов,а также позволить настраивать количество вызовов из данных.
	FNavLocation NavLocation;
	FRandomPointAroundTarget Result;
	
	Result.bIsFound = NavSys->GetRandomReachablePointInRadius(TargetActor->GetActorLocation(), Radius, NavLocation);
	Result.Location = NavLocation.Location;
	
	const float DistanceSquaredToTarget = FVector::DistSquared(TargetActor->GetActorLocation(), NavLocation.Location);
	
	if(SquareMinDistanceToTarget > DistanceSquaredToTarget)
	{
		return RandomPointAroundTarget(NavSys, TargetActor);
	}
	return Result;
}	