// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryContext_Enemy.h"

#include "HomeworkTypes.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "EnvironmentQuery/EnvQueryTypes.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_Actor.h"

void UEnvQueryContext_Enemy::ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const
{
	AActor* QueryOwner = Cast<AActor>(QueryInstance.Owner.Get());

	UBlackboardComponent* Blackboard = UAIBlueprintHelperLibrary::GetBlackboard(QueryOwner);

	const AActor* ContextActor = Cast<AActor>(Blackboard->GetValueAsObject(BB_CurrentTarget));
 
	UEnvQueryItemType_Actor::SetContextHelper(ContextData, ContextActor);
}
