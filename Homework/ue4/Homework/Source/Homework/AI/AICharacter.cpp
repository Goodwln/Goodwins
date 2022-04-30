// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"

#include "Characters/BaseCharacterMovementComponent.h"
#include "Perception/AISense_Damage.h"

AAICharacter::AAICharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	AutoPossessAI = EAutoPossessAI::Disabled;
}

UBehaviorTree* AAICharacter::GetBehaviorTree() const
{
	return BehaviorTree;
}

void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterAttributeComponent()->OnTakeAnyDamageEvent.AddUObject(this, &AAICharacter::TakeAnyDamage);
 
}



void AAICharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                 AController* InstigatedBy, AActor* DamageCauser)
{
	UAISense_Damage::ReportDamageEvent(
		GetWorld(),
		DamagedActor,
		DamageCauser,
		Damage,
		DamageCauser->GetActorLocation(),
		DamagedActor->GetActorLocation());
}
