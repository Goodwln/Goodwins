// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_NextPatrollingPoint.h"

#include "AIController.h"
#include "AI/AICharacter.h"
#include "AI/Controllers/CharacterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CharacterComponents/AIPatrollingComponent.h"

UBTTaskNode_NextPatrollingPoint::UBTTaskNode_NextPatrollingPoint()
{
	NodeName = "NextPatrollingPoint";
}

EBTNodeResult::Type UBTTaskNode_NextPatrollingPoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	ACharacterAIController* Controller = Cast<ACharacterAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if(!IsValid(Controller) || !IsValid(Blackboard))
	{
		return EBTNodeResult::Failed;	
	}

	UAIPatrollingComponent* PatrollingComponent = Controller->GetPatrollingComponent();
	if(!IsValid(PatrollingComponent) || !PatrollingComponent->CanPatrol())
	{
		return EBTNodeResult::Failed;	
	}
	
	bool bIsPatrolling = Blackboard->GetValueAsBool(BB_bIsPatrolling);
	FVector NextWayPoint = bIsPatrolling ? PatrollingComponent->SelectNextWaypoint() :  PatrollingComponent->GetClosestWayPoint() ;
	Blackboard->SetValueAsVector(BB_NextLocation, NextWayPoint);
	Blackboard->SetValueAsBool(BB_bIsPatrolling, true);
 
	return EBTNodeResult::Succeeded;
}
