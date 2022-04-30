// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "AI/Controllers/BaseAIController.h"
#include "Components/CharacterComponents/AIPatrollingComponent.h"
#include "CharacterAIController.generated.h"

class AAICharacter;
UCLASS()
class HOMEWORK_API ACharacterAIController : public ABaseAIController
{
	GENERATED_BODY()

public:
	ACharacterAIController();
	virtual void SetPawn(APawn* InPawn) override;

	virtual void OnPossess(APawn* InPawn) override;
	
	virtual void ActorsPerceptionUpdated(const TArray<AActor*>& UpdatedActors) override;
	UAIPatrollingComponent* GetPatrollingComponent() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Components")
	UAIPatrollingComponent* AIPatrollingComponent;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Movement")
	float TargetReachRadius = 100.0f;
	
	virtual void BeginPlay() override;
private:
	TWeakObjectPtr<AAICharacter> CachedAICharacter;

	bool bIsPatrolling = false;
};
