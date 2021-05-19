// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "BaseCharacter.generated.h"

class UBaseCharacterMovementComponent;

UCLASS(Abstract, NotBlueprintable)
class HOMEWORK_API ABaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ABaseCharacter(const FObjectInitializer& ObjectInitializer);
	
	virtual void MoveForward(float Value) {}
	virtual void MoveRight(float Value) {}
	virtual void Turn(float Value) {}
	virtual void LookUp(float Value) {}

	virtual void ChangeCrouchState();
	
	virtual void StartSprint();
	virtual void StopSprint();

	virtual void StartProne();
	virtual void EndProne();

	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;

	FORCEINLINE UBaseCharacterMovementComponent* GetBaseCharacterMovementComponent() const { return BaseCharacterMovementComponent; }

protected:
	UFUNCTION(BlueprintImplementableEvent, Category = "Character|Movement")
	void OnSprintStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Character|Movement")
	void OnSprintEnd();

	UFUNCTION(BlueprintImplementableEvent, Category = "Characteer|Movement")
	void OnProneStart();

	UFUNCTION(BlueprintImplementableEvent, Category = "Characteer|Movement")
	void OnProneEnd();

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float BaseTurnRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float BaseLookUpRate = 45.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float SprintSpeed = 800.0f;

	// Maximum player stamina
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float MaxStamina = 100.f; 

	// Stamina recovery rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float StaminaRestoreVelocity = 30.f; 

	// Player stamina consumption rate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character|Controls")
	float SprintStaminaConsumptionVelocity = 15.f; 

	bool bIsOutStamina = false;
	
	UBaseCharacterMovementComponent* BaseCharacterMovementComponent;
private:
	
	void TryChangeSprintState(float DeltaSeconds);

	void UpdateStamina(float DeltaSeconds);

	bool bIsSprintRequested = false;

	float CurrentStamina = 50.f;
};