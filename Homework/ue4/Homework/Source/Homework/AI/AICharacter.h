// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/BaseCharacter.h"
#include "AICharacter.generated.h"

/**
 * 
 */
class UBehaviorTree;
class UAIPatrollingComponent;
UCLASS(Blueprintable)
class HOMEWORK_API AAICharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AAICharacter(const FObjectInitializer& ObjectInitializer);
	
	UBehaviorTree* GetBehaviorTree() const;

	virtual void BeginPlay() override;
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UBehaviorTree* BehaviorTree;

	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
					class AController* InstigatedBy, AActor* DamageCauser);
};
