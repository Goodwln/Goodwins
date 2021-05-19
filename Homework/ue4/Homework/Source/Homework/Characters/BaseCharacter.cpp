// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacterMovementComponent.h"

ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{	
	BaseCharacterMovementComponent = StaticCast<UBaseCharacterMovementComponent*>(GetCharacterMovement());
}

void ABaseCharacter::ChangeCrouchState()
{
	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else
	{
		Crouch();
	}
}

void ABaseCharacter::StartSprint()
{
	bIsSprintRequested = true;
	
	if (bIsCrouched)
	{
		UnCrouch();
	}
}

void ABaseCharacter::StopSprint()
{
	bIsSprintRequested = false;
}

void ABaseCharacter::StartProne()
{
	if (BaseCharacterMovementComponent->IsProne())
	{
		BaseCharacterMovementComponent->UnProne();
	}
	else
	{
		BaseCharacterMovementComponent->Prone();
	}
}

void ABaseCharacter::EndProne()
{

}

void ABaseCharacter::Tick(float DeltaSeconds) // 7
{
	Super::Tick(DeltaSeconds);

	UpdateStamina(DeltaSeconds);   
	TryChangeSprintState(DeltaSeconds);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	CurrentStamina = MaxStamina;
}	

/*
All this code is shit, the running animation is looping (the BaseCharacterMovementComponent->IsSprinting() getter of the bIsSprinting variable is responsible for the running animation) you need to figure it out
as it turned out, I am very uncomfortable ... the bIsSprintRequesterd flag was responsible for the animation looping, it was he who made it clear whether the player sat down on shift or not.
*/

void ABaseCharacter::TryChangeSprintState(float DeltaSeconds)
{
	if (!BaseCharacterMovementComponent->GetIsOutOfStamina() && bIsSprintRequested && !BaseCharacterMovementComponent->IsSprinting())
	{
		BaseCharacterMovementComponent->StartSprint();
		OnSprintStart();
	}

	if (!bIsSprintRequested && BaseCharacterMovementComponent->IsSprinting())
	{
		BaseCharacterMovementComponent->StopSprint();
		OnSprintEnd();
	}
}	

void ABaseCharacter::UpdateStamina(float DeltaSeconds)
{
	if (BaseCharacterMovementComponent->IsSprinting())
	{
		CurrentStamina -= SprintStaminaConsumptionVelocity * DeltaSeconds;
		CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
	}

	if (CurrentStamina <= 0)
	{
		BaseCharacterMovementComponent->SetIsOutOfStamina(true);
	}

	if (!BaseCharacterMovementComponent->IsSprinting())
	{
		CurrentStamina += StaminaRestoreVelocity * DeltaSeconds;
		CurrentStamina = FMath::Clamp(CurrentStamina, 0.0f, MaxStamina);
	}

	if (CurrentStamina < MaxStamina)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Yellow, FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina));
	}

	if (CurrentStamina == MaxStamina && BaseCharacterMovementComponent->GetIsOutOfStamina())
	{
		BaseCharacterMovementComponent->SetIsOutOfStamina(false);
	}
}