// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAIController.h"

#include "EngineUtils.h"
#include "Actors/Navigation/PatrollingPath.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/CharacterComponents/AIPatrollingComponent.h"
#include "Homework/AI/AICharacter.h"
#include "Perception/AISense_Sight.h"


ACharacterAIController::ACharacterAIController()
{
	bWantsPlayerState = true;

	AIPatrollingComponent = CreateDefaultSubobject<UAIPatrollingComponent>(TEXT("AIPatrollingComponent"));
}

void ACharacterAIController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	// if(!IsValid(InPawn))
	// {
	// 	CachedAICharacter = nullptr;
	// 	return;
	// }
	//
	// checkf(InPawn->IsA<AAICharacter>(), TEXT("ACharacterAIController can be to use only with AAICharacter"));
	// CachedAICharacter = StaticCast<AAICharacter*>(InPawn);
	// RunBehaviorTree(CachedAICharacter->GetBehaviorTree());
}

void ACharacterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	if(!IsValid(InPawn))
	{
		CachedAICharacter = nullptr;
		return;
	}
	
	checkf(InPawn->IsA<AAICharacter>(), TEXT("ACharacterAIController can be to use only with AAICharacter"));
	CachedAICharacter = StaticCast<AAICharacter*>(InPawn);
	RunBehaviorTree(CachedAICharacter->GetBehaviorTree());
}

void ACharacterAIController::ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors)
{
	Super::ActorsPerceptionUpdated(UpdatedActors);
	 
}

UAIPatrollingComponent* ACharacterAIController::GetPatrollingComponent() const
{
	return AIPatrollingComponent;
}

void ACharacterAIController::BeginPlay()
{
	Super::BeginPlay();

	for(auto elem : TActorRange<APatrollingPath>(GetWorld()))
	{
		if(IsValid(elem))
		{
			if(elem->IsEnablePath())
			{
				elem->SetEnablePath(false);
				AIPatrollingComponent->SetPatrollingPath(elem);
				AIPatrollingComponent->SetPatrollingPathMode(EPatrollingPathChange::Circle);
				break;
			}
		}
	}
}
