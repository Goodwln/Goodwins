// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTaskNode_LaunchThrowable.h"

#include "AIController.h"
#include "Characters/BaseCharacter.h"

UBTTaskNode_LaunchThrowable::UBTTaskNode_LaunchThrowable()
{
	NodeName = "Launch Throwable";
}
 

EBTNodeResult::Type UBTTaskNode_LaunchThrowable::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	if(GetWorld()->GetTimerManager().IsTimerActive(IntervalAfterLaunch))
	{
		return EBTNodeResult::Failed;
	}
	
	AAIController* Controller = OwnerComp.GetAIOwner();
	if(!IsValid(Controller))
	{
		return EBTNodeResult::Failed;
	}
	
	ABaseCharacter* Character = Cast<ABaseCharacter>(Controller->GetPawn());
	if(!IsValid(Character))
	{
		return EBTNodeResult::Failed;
	}
	
	Character->EquipPrimaryItem();

	GetWorld()->GetTimerManager().SetTimer(
		IntervalAfterLaunch, this, &UBTTaskNode_LaunchThrowable::OnTimerAfterLaunchThrowableItem, TimeAfterLaunch,false);
	
	return EBTNodeResult::Succeeded;
}

void UBTTaskNode_LaunchThrowable::OnTimerAfterLaunchThrowableItem()
{
	GetWorld()->GetTimerManager().ClearTimer(IntervalAfterLaunch);
}
