// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BaseCharacterPlayerController.generated.h"


UCLASS()
class HOMEWORK_API ABaseCharacterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void SetPawn(APawn* InPawn) override;

protected:
	virtual void SetupInputComponent() override;

private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void ChangeCrouchState();
	void Jump();
	
	void StartSprint();
	void StopSprint();
	void StartProne();

	TSoftObjectPtr<class ABaseCharacter> CachedBaseCharacter;
};