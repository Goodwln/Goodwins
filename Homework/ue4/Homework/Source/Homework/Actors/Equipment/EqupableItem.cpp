// Fill out your copyright notice in the Description page of Project Settings.


#include "EqupableItem.h"
#include "Homework/Characters/BaseCharacter.h"

AEqupableItem::AEqupableItem()
{
 
}

void AEqupableItem::SetOwner(AActor* NewOwner)
{
	Super::SetOwner(NewOwner);
	if(IsValid(NewOwner))
	{
		checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("AEqupableItem::SetOwner only character can be an owner of an Equipable Item"));
		CachedBaseCharacter = StaticCast<ABaseCharacter*>(GetOwner());
 
	}	
	else
	{
		CachedBaseCharacter = nullptr;
	}
}

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

ABaseCharacter* AEqupableItem::GetCharacterOwner() const
{
	return CachedBaseCharacter.Get();
}
