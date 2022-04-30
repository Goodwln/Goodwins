// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseAIController.h"

#include "Components/CharacterComponents/RespawnComponent.h"
#include "Perception/AIPerceptionComponent.h"

ABaseAIController::ABaseAIController()
{
	PerceptionComponent = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerception"));

	RespawnComponent = CreateDefaultSubobject<URespawnComponent>(TEXT("RespawnComponent"));
}

AActor* ABaseAIController::GetClosestSensedActor(TSubclassOf<UAISense> SenseClass)
{
	if(!IsValid(GetPawn()))
	{
		return nullptr;
	}
	
	TArray<AActor*> SenseActors;
	GetPerceptionComponent()->GetCurrentlyPerceivedActors(SenseClass,SenseActors);
	
	AActor* ClosestActor = nullptr;
	float MinSquaredDistance = FLT_MAX;

	const FVector PawnLocation = GetPawn()->GetActorLocation();

	for(AActor* SeenActor : SenseActors)
	{
		const float CurrentSquaredDistance = (PawnLocation - SeenActor->GetActorLocation()).SizeSquared();
		if(CurrentSquaredDistance < MinSquaredDistance)
		{
			MinSquaredDistance = CurrentSquaredDistance;
			ClosestActor = SeenActor;
		}
	}

	return ClosestActor;
}
