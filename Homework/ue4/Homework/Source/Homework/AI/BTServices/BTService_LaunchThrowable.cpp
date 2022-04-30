// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_LaunchThrowable.h"

#include "AI/AICharacter.h"
#include "AI/Controllers/CharacterAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTService_LaunchThrowable::UBTService_LaunchThrowable()
{
	NodeName = "Launch Throwable";
}

void UBTService_LaunchThrowable::PostLoad()
{
	Super::PostLoad();

	SquareMinDistanceThrowable = MinDistanceThrowable * MinDistanceThrowable;
	SquareMaxDistanceThrowable = MaxDistanceThrowable * MaxDistanceThrowable;
}

bool UBTService_LaunchThrowable::CanLaunchThrowable(const UBehaviorTreeComponent& OwnerComp, ABaseCharacter*& Character) const
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	const UBlackboardComponent* Blackboard = Controller->GetBlackboardComponent();
	if(!IsValid(Controller) || !IsValid(Blackboard))
	{
		return false;
	}

	Character = Cast<ABaseCharacter>(Controller->GetPawn());
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
#if WITH_EDITOR
void UBTService_LaunchThrowable::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	
	if(MaxDistanceThrowable < MinDistanceThrowable)
	{
		MaxDistanceThrowable = MinDistanceThrowable;
	}
}
#endif

void UBTService_LaunchThrowable::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Interval = DefaultInterval;
	AAIController* Controller = OwnerComp.GetAIOwner();
	ABaseCharacter* Character = Cast<ABaseCharacter>(Controller->GetPawn());
	if (CanLaunchThrowable(OwnerComp, Character))
	{
		Interval = AfterLaunchInterval;
		Character->EquipPrimaryItem();
	}
	
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
