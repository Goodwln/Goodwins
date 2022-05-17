// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_SearchTarget.h"

#include "AIController.h"
#include "AI/AICharacter.h"
#include "AI/Controllers/BaseAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Managers/TeamManager.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Sight.h"
#include "Utils/HomeworkUtils.h"

UBTService_SearchTarget::UBTService_SearchTarget()
{
	NodeName = "Search Target";
}

void UBTService_SearchTarget::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	ABaseAIController* Controller = Cast<ABaseAIController>(OwnerComp.GetAIOwner());
	UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if(!IsValid(Controller) || !IsValid(Blackboard))
	{
		return;
	}
	
	AActor* Actor = Controller->GetClosestSensedActor(UAISense_Damage::StaticClass());

	Actor = IsValid(Actor) ? Actor->GetOwner() : Controller->GetClosestSensedActor(UAISense_Sight::StaticClass());
	
	if(!IsValid(Actor))
	{
		 Controller->ClearFocus(EAIFocusPriority::Gameplay);
		Blackboard->SetValueAsObject(BB_CurrentTarget, Actor);
		return;
	}

	ABaseCharacter* Character = Cast<ABaseCharacter>(Controller->GetPawn());
	if(!IsValid(Character))
	{
		Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr);
		Controller->ClearFocus(EAIFocusPriority::Gameplay);
		return;
	}

	ABaseCharacter* UnknownCharacter = Cast<ABaseCharacter>(Actor);
	AController* UnknownController = nullptr;
	if(IsValid(UnknownCharacter))
	{
		UnknownController = UnknownCharacter->GetController();
	}
	
	if(!IsValid(UnknownController))
	{
		Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr);
		Controller->ClearFocus(EAIFocusPriority::Gameplay);
		return;
	}

	if(!HomeworkUtils::AreEnemies(Controller, UnknownController))
	{
		Character->StopFire();
		Blackboard->SetValueAsObject(BB_CurrentTarget, nullptr);
		Controller->ClearFocus(EAIFocusPriority::Gameplay);
		return;
	}
 
	Blackboard->SetValueAsObject(BB_CurrentTarget, Actor);
	Blackboard->SetValueAsBool(BB_bIsPatrolling, false);
	Controller->SetFocus(Actor, EAIFocusPriority::Gameplay);
}
