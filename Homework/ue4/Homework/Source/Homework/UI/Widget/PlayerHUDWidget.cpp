// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "ReticleWidget.h"
#include "AmmoWidget.h"
#include "GameModeLearning.h"
#include "RoundInfoWidget.h"
#include "SpectatorWidget.h"
#include "ThrowableWidget.h"
#include "Characters/BaseCharacter.h"
#include "Characters/Controllers/BaseCharacterPlayerController.h"
#include "Components/CharacterComponents/RespawnComponent.h"
#include "UI/Widget/WidgetCharacterAttributes.h"


DEFINE_LOG_CATEGORY_STATIC(LogPlayerHUDWidget, All, All);

void UPlayerHUDWidget::InitializeWidget()
{
	CacheController = GetOwningPlayer();

		ABaseCharacter* Character = Cast<ABaseCharacter>(GetOwningPlayerPawn());
		if (IsValid(Character))
		{
			InitReticleWidget(Character);
			InitAmmoWidget(Character);
			InitStaminaWidget(Character);
			InitOxygenWidget(Character);
			InitHealthWidget(Character);
			InitThrowableWidget(Character);
			InitRoundInfoWidget();
			InitSpectatorWidget();
		}
}

void UPlayerHUDWidget::WasSpectatorMode(bool IsVisibility)
{
	const ESlateVisibility CurrentVisibility = !IsVisibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	
	GetReticleWidget()->SetVisibility(CurrentVisibility);
	GetAmmoWidget()->SetVisibility(CurrentVisibility);
	GetStaminaAttributeWidget()->SetVisibility(CurrentVisibility);
	GetOxygenAttributeWidget()->SetVisibility(CurrentVisibility);
	GetHealthAttributeWidget()->SetVisibility(CurrentVisibility);
	GetThrowableWidget()->SetVisibility(CurrentVisibility);
	GetRoundInfoWidget()->SetVisibility(CurrentVisibility);
}

void UPlayerHUDWidget::OnNewPawn(APawn* NewPawn)
{
	InitializeWidget();
}

bool UPlayerHUDWidget::Initialize()
{
	if(GetOwningPlayer())
	{
		GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UPlayerHUDWidget::OnNewPawn);
	}
	
	return Super::Initialize();
}


UReticleWidget* UPlayerHUDWidget::GetReticleWidget()
{
	return WidgetTree->FindWidget<UReticleWidget>(ReticleWidgetName);
}

UAmmoWidget* UPlayerHUDWidget::GetAmmoWidget()
{
	return WidgetTree->FindWidget<UAmmoWidget>(AmmoWidgetName);
}

UWidgetCharacterAttributes* UPlayerHUDWidget::GetStaminaAttributeWidget()
{
	return WidgetTree->FindWidget<UWidgetCharacterAttributes>(StaminaAttributeWidgetName);
}

UWidgetCharacterAttributes* UPlayerHUDWidget::GetOxygenAttributeWidget()
{
	return WidgetTree->FindWidget<UWidgetCharacterAttributes>(OxygenAttributeWidgetName);
}

UWidgetCharacterAttributes* UPlayerHUDWidget::GetHealthAttributeWidget()
{
	return WidgetTree->FindWidget<UWidgetCharacterAttributes>(HealthAttributeWidgetName);
}

UThrowableWidget* UPlayerHUDWidget::GetThrowableWidget()
{
	return WidgetTree->FindWidget<UThrowableWidget>(ThrowableWidgetName);
}

URoundInfoWidget* UPlayerHUDWidget::GetRoundInfoWidget()
{
	return WidgetTree->FindWidget<URoundInfoWidget>(RoundInfoWidgetName);
}

USpectatorWidget* UPlayerHUDWidget::GetTimeRespawnInfoWidget()
{
	return WidgetTree->FindWidget<USpectatorWidget>(TimeRespawnInfoWidgetName);
}

void UPlayerHUDWidget::UpdatePlayerStats(AController* KillerController, AController* VictimController, int32 Kills,
	int32 Deaths)
{
	if(!IsValid(CacheController))
	{
		return;
	}

	if(CacheController != VictimController && KillerController == CacheController)
	{
		OnUpdateKillStatsInfoEvent.Broadcast(Kills);
	}
	
	if(CacheController == VictimController && KillerController != CacheController)
	{
		OnUpdateDeathStatsInfoEvent.Broadcast(Deaths);
	}
}

void UPlayerHUDWidget::InitReticleWidget(ABaseCharacter* Character)
{
	UReticleWidget* ReticleWidget = GetReticleWidget();
	if (IsValid(ReticleWidget) )
	{
		Character->OnAimingStateChanged.AddUFunction(ReticleWidget, FName("OnAimingStateChanged"));

		UCharacterEquipmentComponent* CharacterEquipment = Character->GetCharacterEquipmentComponent_Mutable();
 
			CharacterEquipment->OnEquippedItemChangedEvent.AddUFunction(
				ReticleWidget, FName("OnEquipItemChanged"));
 
	}
}

void UPlayerHUDWidget::InitAmmoWidget(ABaseCharacter* Character)
{
	UAmmoWidget* AmmoWidget = GetAmmoWidget();
	if (IsValid(AmmoWidget))
	{
		UCharacterEquipmentComponent* CharacterEquipment = Character->GetCharacterEquipmentComponent_Mutable();
 
			CharacterEquipment->OnCurrentWeaponAmmoChangedEvent.AddUFunction(AmmoWidget, FName("UpdateAmmoCount"));
 
	}
}

void UPlayerHUDWidget::InitStaminaWidget(ABaseCharacter* Character)
{
	UWidgetCharacterAttributes* CharacterAttributesStamina = GetStaminaAttributeWidget();
	if (IsValid(CharacterAttributesStamina))
	{
		UCharacterAttributeComponent* CharacterAttributeComponent = Character->
			GetCharacterAttributeComponent();
 
			CharacterAttributeComponent->OnChangeStaminaEvent.AddUFunction(
				CharacterAttributesStamina, FName("OnAttributeUpdate"));
	 
	}
}

void UPlayerHUDWidget::InitOxygenWidget(ABaseCharacter* Character)
{
	UWidgetCharacterAttributes* CharacterAttributesOxygen = GetOxygenAttributeWidget();
	if (IsValid(CharacterAttributesOxygen))
	{
		UCharacterAttributeComponent* CharacterAttributeComponent = Character->
			GetCharacterAttributeComponent();
		CharacterAttributeComponent->OnChangeOxygenEvent.AddUFunction(
			CharacterAttributesOxygen, FName("OnAttributeUpdate"));
	}
}

void UPlayerHUDWidget::InitHealthWidget(ABaseCharacter* Character)
{
	UWidgetCharacterAttributes* CharacterAttributesHealth = GetHealthAttributeWidget();
	if (IsValid(CharacterAttributesHealth))
	{
		UCharacterAttributeComponent* CharacterAttributeComponent = Character->
			GetCharacterAttributeComponent();
		CharacterAttributeComponent->OnChangeHealthEvent.AddUFunction(
			CharacterAttributesHealth, FName("OnAttributeUpdate"));
	}
}

void UPlayerHUDWidget::InitThrowableWidget(ABaseCharacter* Character)
{
	UThrowableWidget* ThrowableWidget = GetThrowableWidget();
	if (IsValid(ThrowableWidget))
	{
		UCharacterEquipmentComponent* CharacterEquipment = Character->GetCharacterEquipmentComponent_Mutable();
		CharacterEquipment->OnCurrentCapacityThrowableChangedEvent.AddUFunction(
			ThrowableWidget, FName("CapacityThrowable"));
	}
}

void UPlayerHUDWidget::InitRoundInfoWidget()
{
	URoundInfoWidget* RoundInfoWidget = GetRoundInfoWidget();
	if (IsValid(RoundInfoWidget))
	{
		if (IsValid(CacheController))
		{
			AGameModeLearning* GameMode = Cast<AGameModeLearning>(GetWorld()->GetAuthGameMode());
			if (IsValid(GameMode))
			{
 				GameMode->OnStatsPlayerEvent.AddUFunction(this, FName("UpdatePlayerStats"));
				OnUpdateDeathStatsInfoEvent.AddUFunction(RoundInfoWidget, FName("DeathsInfo"));
				OnUpdateKillStatsInfoEvent.AddUFunction(RoundInfoWidget, FName("KillsInfo"));

				GameMode->OnRoundInfoEvent.AddUFunction(RoundInfoWidget, FName("RoundInfo"));
				GameMode->OnRoundTimeEvent.AddUFunction(RoundInfoWidget, FName("TimeInfo"));
			}
		}
	}
}

void UPlayerHUDWidget::InitSpectatorWidget()
{
	USpectatorWidget* SpectatorWidget = GetTimeRespawnInfoWidget();
	if (IsValid(SpectatorWidget))
	{
		const ABaseCharacterPlayerController* PlayerController = Cast<ABaseCharacterPlayerController>(GetOwningPlayer());
		if (IsValid(PlayerController))
		{
			URespawnComponent* RespawnComponent = PlayerController->GetRespawnComponent();
			if (IsValid(RespawnComponent))
			{
				RespawnComponent->OnTimerToRespawnEvent.AddUFunction(SpectatorWidget, FName("TimeRespawnInfo"));
				RespawnComponent->OnActivatedSpectatorModeEvent.AddUFunction(SpectatorWidget, FName("VisibilityWidget"));
				SpectatorWidget->OnVisibilitySpectatorWidgetEvent.AddUFunction(this, FName("WasSpectatorMode"));
			}
		}
	}
}
 

 