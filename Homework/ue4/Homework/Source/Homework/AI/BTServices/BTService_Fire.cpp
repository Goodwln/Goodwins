// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_Fire.h"

#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Characters/BaseCharacter.h"

UBTService_Fire::UBTService_Fire()
{
	NodeName = "Fire";
}

void UBTService_Fire::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComponent = AIController->GetBlackboardComponent();

	if(!IsValid(AIController) || !IsValid(BlackboardComponent)) return;
	
	checkf(AIController->GetPawn()->IsA<ABaseCharacter>(), TEXT("UBTService_Fire can be to use only with an ABaseCharacter Class"));
	ABaseCharacter* Character = StaticCast<ABaseCharacter*>(AIController->GetPawn());
	if(!IsValid(Character)) return;
	
	const UCharacterEquipmentComponent* EquipmentComponent = Character->GetCharacterEquipmentComponent();
	const ARangeWeaponItem* RangeWeapon = EquipmentComponent->GetCurrentRangeWeapon();
	if(!IsValid(RangeWeapon)) return;
	
	const AActor* CurrentTarget = Cast<AActor>(BlackboardComponent->GetValueAsObject(BB_CurrentTarget));
	
	if(!IsValid(CurrentTarget))
	{
		return;
	}

	const float DistanceSquare = FVector::DistSquared(CurrentTarget->GetActorLocation(), Character->GetActorLocation());
	if (DistanceSquare > FMath::Square(MaxFireDistance))
	{
		Character->StopFire();
		return;
	}
		
	if(!RangeWeapon->IsReloading() || RangeWeapon->IsFiring())
	{
		Character->StartFire();
	}
}
