// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterPlayerController.h"
#include "../BaseCharacter.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "UI/Widget/PlayerHUDWidget.h"
#include "UI/Widget/ReticleWidget.h"
#include "UI/Widget/AmmoWidget.h"
#include "UI/Widget/WidgetCharacterAttributes.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/CharacterComponents/RespawnComponent.h"

ABaseCharacterPlayerController::ABaseCharacterPlayerController()
{
	RespawnComponent = CreateDefaultSubobject<URespawnComponent>(TEXT("RespawnComponent"));
}



void ABaseCharacterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	CachedBaseCharacter = Cast<ABaseCharacter>(InPawn);

	OnNewPawn.Broadcast(InPawn);
	CreateAndInitializeWidget();
}

void ABaseCharacterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	InputComponent->BindAxis("MoveForward", this, &ABaseCharacterPlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &ABaseCharacterPlayerController::MoveRight);
	
	InputComponent->BindAxis("Turn", this, &ABaseCharacterPlayerController::Turn);
	InputComponent->BindAxis("LookUp", this, &ABaseCharacterPlayerController::LookUp);

	InputComponent->BindAxis("SwimForward", this, &ABaseCharacterPlayerController::SwimForward);
	InputComponent->BindAxis("SwimRight", this, &ABaseCharacterPlayerController::SwimRight);
	InputComponent->BindAxis("SwimUp", this, &ABaseCharacterPlayerController::SwimUp);

	InputComponent->BindAxis("WallRun", this, &ABaseCharacterPlayerController::WallRun);

	InputComponent->BindAxis("ClimbLadderUp", this, &ABaseCharacterPlayerController::ClimbLadderUp);
	InputComponent->BindAction("InteractWithLadder", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::InteractWithLadder);

	InputComponent->BindAction("InteractWithZipline", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::InteractWithZipline);

	InputComponent->BindAction("Mantle", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::Mantle);

	InputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::Jump);

	InputComponent->BindAction("Slide", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::StartSlide);
	InputComponent->BindAction("Slide", EInputEvent::IE_Released, this, &ABaseCharacterPlayerController::StopSlide);
	
	InputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::ChangeCrouchState);
	
	InputComponent->BindAction("ClassicProne", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::ChangeProneState);
	
	InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::StartSprint);
	InputComponent->BindAction("Sprint", EInputEvent::IE_Released, this, &ABaseCharacterPlayerController::StopSprint);
	
	InputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::FireStart);
	InputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ABaseCharacterPlayerController::FireStop);

	InputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::StartAiming);
	InputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ABaseCharacterPlayerController::StopAiming);

	InputComponent->BindAction("Reload", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::Reload);

	InputComponent->BindAction("NextItem", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::NextItem);
	InputComponent->BindAction("PreviousItem", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::PreviousItem);
	
	InputComponent->BindAction("EquipPrimaryItem", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::EquipPrimaryItem);

	InputComponent->BindAction("SecondaryFire", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::SecondaryFire);

	InputComponent->BindAction("SecondaryMeleeAttack", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::SecondaryMeleeAttack);
	InputComponent->BindAction("PrimaryMeleeAttack", EInputEvent::IE_Pressed, this, &ABaseCharacterPlayerController::PrimaryMeleeAttack);
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

void ABaseCharacterPlayerController::Mantle()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Mantle();
	}
}

void ABaseCharacterPlayerController::Jump()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Jump();
	}
}

void ABaseCharacterPlayerController::ClimbLadderUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ClimbLadderUp(Value);
	}
}

void ABaseCharacterPlayerController::InteractWithLadder()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithLadder();
	}
}

void ABaseCharacterPlayerController::InteractWithZipline()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->InteractWithZipline();
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

void ABaseCharacterPlayerController::SwimForward(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimForward(Value);
	}
}

void ABaseCharacterPlayerController::SwimRight(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimRight(Value);
	}
}

void ABaseCharacterPlayerController::SwimUp(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SwimUp(Value);
	}
}

void ABaseCharacterPlayerController::WallRun(float Value)
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->WallRun(Value);
	}
}

void ABaseCharacterPlayerController::StartSlide()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartSlide();
	}
}

void ABaseCharacterPlayerController::StopSlide()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopSlide();
	}
}

void ABaseCharacterPlayerController::ChangeProneState()
{
	if (CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->ChangeProneState();
	}
}

void ABaseCharacterPlayerController::FireStart()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartFire();	
	}
}

void ABaseCharacterPlayerController::FireStop()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopFire();	
	}
}

void ABaseCharacterPlayerController::StartAiming()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StartAiming();
	}
}

void ABaseCharacterPlayerController::StopAiming()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->StopAiming();
	}
}

void ABaseCharacterPlayerController::Reload()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->Reload();
	}
}

void ABaseCharacterPlayerController::NextItem()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->NextItem();
	}
}

void ABaseCharacterPlayerController::PreviousItem()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PreviousItem();
	}
}

void ABaseCharacterPlayerController::EquipPrimaryItem()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->EquipPrimaryItem();
	}
}

void ABaseCharacterPlayerController::SecondaryFire()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SecondaryFire();
	}
}

void ABaseCharacterPlayerController::PrimaryMeleeAttack()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->PrimaryMeleeAttack();
	}
}

void ABaseCharacterPlayerController::SecondaryMeleeAttack()
{
	if(CachedBaseCharacter.IsValid())
	{
		CachedBaseCharacter->SecondaryMeleeAttack();
	}
}

void ABaseCharacterPlayerController::CreateAndInitializeWidget()
{
	if(!IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHudWidgetClass);

		if(IsValid(PlayerHUDWidget))
		{
			PlayerHUDWidget->AddToViewport();
			PlayerHUDWidget->CreateAndInitializeWidget(CachedBaseCharacter.Get(),this);
		 
		}
	}
}
   