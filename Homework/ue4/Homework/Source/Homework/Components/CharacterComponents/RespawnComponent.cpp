// Fill out your copyright notice in the Description page of Project Settings.


#include "RespawnComponent.h"

#include "GameModeLearning.h"

// Sets default values for this component's properties
URespawnComponent::URespawnComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void URespawnComponent::Respawn(int32 RespawnTime)
{
	if(!IsValid(GetWorld()))
	{
		return;
	}
	RespawnCountDown = RespawnTime;
	OnTimerToRespawnEvent.Broadcast(RespawnCountDown--);
	OnActivatedSpectatorModeEvent.Broadcast(true);
	GetWorld()->GetTimerManager().SetTimer(RespawnTimerHandle, this, &URespawnComponent::RespawnTimerUpdate, 1.f, true);
}

bool URespawnComponent::IsRespawnActive() const
{
	return GetWorld()->GetTimerManager().IsTimerActive(RespawnTimerHandle);
}

void URespawnComponent::RespawnTimerUpdate()
{
	OnTimerToRespawnEvent.Broadcast(RespawnCountDown);
	if(RespawnCountDown-- == 0)
	{
		if(!IsValid(GetWorld()))
		{
			return;
		}
		OnActivatedSpectatorModeEvent.Broadcast(false);
		GetWorld()->GetTimerManager().ClearTimer(RespawnTimerHandle);
		
		AGameModeLearning* GameMode = Cast<AGameModeLearning>(GetWorld()->GetAuthGameMode());
		if(IsValid(GameMode))
		{
			GameMode->RespawnRequest(Cast<AController>(GetOwner()));
		}
	}
}



