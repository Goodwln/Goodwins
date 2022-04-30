// Fill out your copyright notice in the Description page of Project Settings.


#include "BTDecorator_CurrentDistance.h"
#include "AI/AICharacter.h"
#include "AI/Controllers/CharacterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTDecorator_CurrentDistance::UBTDecorator_CurrentDistance()
{
	NodeName = "CurrentDistance";
}

bool UBTDecorator_CurrentDistance::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp,
                                                              uint8* NodeMemory) const
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	const UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if(!IsValid(Controller) || !IsValid(Blackboard))
	{
		return false;
	}

	ABaseCharacter* Character = Cast<ABaseCharacter>(Controller->GetPawn());
	if(!IsValid(Character) || !Character->GetCharacterEquipmentComponent()->IsAvailableThrowable())
	{
		return false;
	}
	
	const AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(BB_CurrentTarget));
	if(!IsValid(Target))
	{
		return false;
	}
	
	const FVector TargetLocation = Target->GetActorLocation();
	const FVector OwnerLocation = Character->GetActorLocation();
	
	const float SquareDistanceToTarget = FVector::DistSquared(TargetLocation, OwnerLocation);
	if(SquareDistanceToTarget < SquareMinDistanceThrowable || SquareDistanceToTarget > SquareMaxDistanceThrowable)
	{
		return false;
	}
	return true;
}

void UBTDecorator_CurrentDistance::PostLoad()
{
	Super::PostLoad();

	SquareMinDistanceThrowable = MinDistanceThrowable * MinDistanceThrowable;
	SquareMaxDistanceThrowable = MaxDistanceThrowable * MaxDistanceThrowable;
}

#if WITH_EDITOR
void UBTDecorator_CurrentDistance::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if(MinDistanceThrowable > MaxDistanceThrowable)
	{
		MaxDistanceThrowable = MinDistanceThrowable;
	}
}
#endif
