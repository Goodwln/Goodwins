// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerHUDWidget.h"

#include "Blueprint/WidgetTree.h"
#include "ReticleWidget.h"
#include "AmmoWidget.h"
#include "ThrowableWidget.h"
#include "Characters/BaseCharacter.h"
#include "UI/Widget/WidgetCharacterAttributes.h"

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


void UPlayerHUDWidget::CreateAndInitializeWidget(ABaseCharacter* CharacterOwner)
{
	if (IsValid(CharacterOwner))
	{
		UReticleWidget* ReticleWidget = GetReticleWidget();
		if (IsValid(ReticleWidget))
		{									
			CharacterOwner->OnAimingStateChanged.AddUFunction(ReticleWidget, FName("OnAimingStateChanged"));
			UCharacterEquipmentComponent* CharacterEquipment = CharacterOwner->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnEquippedItemChangedEvent.AddUFunction(ReticleWidget,  FName("OnEquipItemChanged"));
		}

		UAmmoWidget* AmmoWidget = GetAmmoWidget();
		if (IsValid(AmmoWidget))
		{
			UCharacterEquipmentComponent* CharacterEquipment = CharacterOwner->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnCurrentWeaponAmmoChangedEvent.AddUFunction(AmmoWidget, FName("UpdateAmmoCount"));
		}

		UWidgetCharacterAttributes* CharacterAttributesStamina = GetStaminaAttributeWidget();
		if (IsValid(CharacterAttributesStamina))
		{
			UCharacterAttributeComponent* CharacterAttributeComponent = CharacterOwner->
				GetCharacterAttributeComponent();
			CharacterAttributeComponent->OnChangeStaminaEvent.AddUFunction(
				CharacterAttributesStamina, FName("OnAttributeUpdate"));
		}

		UWidgetCharacterAttributes* CharacterAttributesOxygen = GetOxygenAttributeWidget();
		if (IsValid(CharacterAttributesOxygen))                            
		{
			UCharacterAttributeComponent* CharacterAttributeComponent = CharacterOwner->
				GetCharacterAttributeComponent();
			CharacterAttributeComponent->OnChangeOxygenEvent.AddUFunction(
				CharacterAttributesOxygen, FName("OnAttributeUpdate"));
		}

		UWidgetCharacterAttributes* CharacterAttributesHealth = GetHealthAttributeWidget();
		if (IsValid(CharacterAttributesOxygen))
		{
			UCharacterAttributeComponent* CharacterAttributeComponent = CharacterOwner->
				GetCharacterAttributeComponent();
			CharacterAttributeComponent->OnChangeHealthEvent.AddUFunction(
				CharacterAttributesHealth, FName("OnAttributeUpdate"));
		}

		UThrowableWidget* ThrowableWidget = GetThrowableWidget();
		if (IsValid(CharacterAttributesOxygen))
		{
			UCharacterEquipmentComponent* CharacterEquipment = CharacterOwner->GetCharacterEquipmentComponent_Mutable();
			CharacterEquipment->OnCurrentCapacityThrowableChangedEvent.AddUFunction(
				ThrowableWidget, FName("CapacityThrowable"));
		}
	}
}
