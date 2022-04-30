// Fill out your copyright notice in the Description page of Project Settings.


#include "EnvQueryTest_PickupCouldBeTaken.h"

#include "Actors/Pickups/BasePickup.h"
#include "EnvironmentQuery/Items/EnvQueryItemType_ActorBase.h"

UEnvQueryTest_PickupCouldBeTaken::UEnvQueryTest_PickupCouldBeTaken(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	Cost = EEnvTestCost::Low;
	ValidItemType = UEnvQueryItemType_ActorBase::StaticClass();
	SetWorkOnFloatValues(false);
}

void UEnvQueryTest_PickupCouldBeTaken::RunTest(FEnvQueryInstance& QueryInstance) const
{
	const UObject* DataOwner = QueryInstance.Owner.Get();
	BoolValue.BindData(DataOwner, QueryInstance.QueryID);
	const bool bWantsHit = BoolValue.GetValue();
	
	for (FEnvQueryInstance::ItemIterator It(this, QueryInstance); It; ++It)
	{
		AActor* ItemActor = GetItemActor(QueryInstance, It.GetIndex());
		ABasePickup* PickupActor = Cast<ABasePickup>(ItemActor);
		if(!IsValid(PickupActor))
		{
			continue;
		}

		const bool bCouldBeTaken = PickupActor->CouldBeTaken();
		It.SetScore(TestPurpose, FilterType, bCouldBeTaken, bWantsHit);
	}
}
