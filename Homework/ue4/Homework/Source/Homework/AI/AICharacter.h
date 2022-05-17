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
class UWidgetComponent;

UCLASS(Blueprintable)
class HOMEWORK_API AAICharacter : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AAICharacter(const FObjectInitializer& ObjectInitializer);
	
	UBehaviorTree* GetBehaviorTree() const;

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
protected:
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UBehaviorTree* BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	UWidgetComponent* HealthWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	float HealthVisibilityDistance = 1000.f;

	
	UFUNCTION()
	void TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
					 AController* InstigatedBy, AActor* DamageCauser);

	virtual void OnChangeHealth(float Current, float Max, FDefaultAttributeProperty Attribute) override;
	
private:
	void UpdateHealthWidgetVisibility();

	
};
