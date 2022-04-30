// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseAIController.h"
#include "AITurretController.generated.h"

class ATurret;

UCLASS()
class HOMEWORK_API AAITurretController : public ABaseAIController
{
	GENERATED_BODY()
public:
	virtual void SetPawn(APawn* InPawn) override;
	
	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;

private:
	
	TWeakObjectPtr<ATurret> CachedTurret;
};
