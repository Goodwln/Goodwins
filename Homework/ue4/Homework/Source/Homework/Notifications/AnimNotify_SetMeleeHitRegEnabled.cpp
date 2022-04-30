// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_SetMeleeHitRegEnabled.h"
#include "Homework/Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "Characters/BaseCharacter.h"

void UAnimNotify_SetMeleeHitRegEnabled::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	ABaseCharacter* CharacterOwner = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if(!IsValid(CharacterOwner))
	{
		return;
	}

	AMeleeWeaponItem* MeleeWeaponItem = CharacterOwner->GetCharacterEquipmentComponent()->GetCurrentMeleeWeaponItem();
	if(IsValid(MeleeWeaponItem))
	{
		MeleeWeaponItem->SetIsHitRegistrationEnabled(bIsHitRegistrationEnabled);
	}
}
