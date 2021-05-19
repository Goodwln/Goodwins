// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacterMovementComponent.generated.h"

class ABaseCharacter;

UCLASS()
class HOMEWORK_API UBaseCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:
	UBaseCharacterMovementComponent();
	FORCEINLINE bool IsSprinting() { return bIsSprinting; }

	FORCEINLINE bool IsProne() { return bIsProne; }

	virtual float GetMaxSpeed() const override;
	
	virtual void Prone();
	virtual void UnProne();

	void StartSprint();
	void StopSprint();
	void SetIsOutOfStamina(bool bIsOutOfStamina_In);

	FORCEINLINE bool GetIsOutOfStamina() { return bIsOutOfStamina; };

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Prone", meta = (ClampMin = 1.f))
	float ProneCapsuleRadius = 40.f; // Capsule radius in a prone state
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Prone", meta = (ClampMin = 1.f))
	float ProneCapsuleHalfHeight = 40.f; // Capsule half height in prone state
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Sprint", meta = (ClampMin = 0.f, UIMin = 0.f))
	float SprintSpeed = 1600;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Stamina", meta = (ClampMin = 0.f, UIMin = 0.f))
	float OutStaminaWalkSpeed = 100.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Character movement|Prone", meta = (ClampMin = 0.f, UIMax = 0.f))
	float MaxProneSpeed = 100.0f;



private:
	bool bIsSprinting = false;
	bool bIsOutOfStamina = false;
	bool bIsProne = false;
	float OldUnscaledHalfHeight = 0.f;
	float OldUnscaledRadius = 0.f;
};