// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RespawnComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTimerToRespawnEventSignature, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnActivatedSpectatorModeEventSignature, bool);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK_API URespawnComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URespawnComponent();

	void Respawn(int32 RespawnTime);

	bool IsRespawnActive() const;
	FOnTimerToRespawnEventSignature OnTimerToRespawnEvent;
	FOnActivatedSpectatorModeEventSignature OnActivatedSpectatorModeEvent;
 private:
	FTimerHandle RespawnTimerHandle;
	int32 RespawnCountDown = 0;

	void RespawnTimerUpdate();
	
};
