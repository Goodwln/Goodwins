// Fill out your copyright notice in the Description page of Project Settings.


#include "EqupableItem.h"


FName AEqupableItem::GetUnEquippedSocketName() const
{
	return  UnEquippedSocketName;
}

FName AEqupableItem::GetEquippedSocketName() const
{
	return  EquippedSocketName;
}

UAnimMontage* AEqupableItem::GetCharacterEquipmentAnimMontage() const
{
	return CharacterEquipmentAnimMontage;
}

void AEqupableItem::Equip()
{
	if(OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(true);
	}
}

void AEqupableItem::UnEquip()
{
	if(OnEquipmentStateChanged.IsBound())
	{
		OnEquipmentStateChanged.Broadcast(false);
	}
}

ECurrentReticle AEqupableItem::GetCurrentReticle() const
{
	return CurrentReticle;			
}
