// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterPlayerController.h"
#include "../BaseCharacter.h"

void ABaseCharacterPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);
	CachedBaseCharacter = Cast<ABaseCharacter>(InPawn);
}

void ABaseCharacterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &ABaseCharacterPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABaseCharacterPlayerController::MoveRight);
	InputComponent->BindAxis("Turn", this, &ABaseCharacterPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &ABaseCharacterPlayerController::LookUp);
	InputComponent->BindAction("Jump", EInputEvent::IE_DoubleClick, this, &ABaseCharacterPlayerController::Jump);
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::ChangeCrouchState);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ABaseCharacterPlayerController::StopSprint);
	InputComponent->BindAction("Prone", EInputEvent::IE_Released, this, &ABaseCharacterPlayerController::StartProne);

}

void ABaseCharacterPlayerController::MoveForward(float val)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveForward(val);
	}
}

void ABaseCharacterPlayerController::MoveRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->MoveRight(Value);
	}
}

void ABaseCharacterPlayerController::Turn(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Turn(Value);
	}
}

void ABaseCharacterPlayerController::LookUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->LookUp(Value);
	}
}

void ABaseCharacterPlayerController::ChangeCrouchState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeCrouchState();
	}
}

void ABaseCharacterPlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void ABaseCharacterPlayerController::StartSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSprint();
	}
}

void ABaseCharacterPlayerController::StopSprint()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSprint();
	}
}

void ABaseCharacterPlayerController::StartProne()
{
	if (CachedBaseCharacter->IsValidLowLevel())
	{
		CachedBaseCharacter->StartProne();
	}
}
