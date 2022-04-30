// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeWeaponItem.h"

#include "Components/Weapon/MeleeHitRegistrator.h"
#include "Homework/Characters/BaseCharacter.h"

AMeleeWeaponItem::AMeleeWeaponItem()
{
	EquippedSocketName = SocketCharacterWeapon;
}

void AMeleeWeaponItem::StartAttack(EMeleeAttackType AttackType)
{
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if(!IsValid(CharacterOwner))
	{
		return;
	}

	CurrentAttack = Attacks.Find(AttackType);
	if(CurrentAttack && IsValid(CurrentAttack->AttackMontage))
	{
		UAnimInstance* CharacterAnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
		if(IsValid(CharacterAnimInstance))
		{
			float Duration =  CharacterAnimInstance->Montage_Play(CurrentAttack->AttackMontage, 1.f, EMontagePlayReturnType::Duration);
			GetWorld()->GetTimerManager().SetTimer(AttackTimer, this, &AMeleeWeaponItem::OnAttackTimerElapsed, Duration, false);
		}
		else
		{
			OnAttackTimerElapsed();
		}
	}
}

void AMeleeWeaponItem::SetIsHitRegistrationEnabled(bool bIsRegistrationEnabled)
{
	HitActors.Empty();
	for(UMeleeHitRegistrator* HitRegistrator : HitRegistrators)
	{
		HitRegistrator->SetIsHitRegistrationEnable(bIsRegistrationEnabled);
	}
}

void AMeleeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	GetComponents<UMeleeHitRegistrator>(HitRegistrators);
	for (UMeleeHitRegistrator* HitRegistrator : HitRegistrators)
	{
		HitRegistrator->OnMeleeHitRegistred.AddDynamic(this, &AMeleeWeaponItem::ProcessHit);
	}
}

void AMeleeWeaponItem::ProcessHit(const FHitResult& HitResult, const FVector& Direction)
{
	if(!CurrentAttack)
	{
		return;
	}
	
	AActor* HitActor = HitResult.GetActor();
	if(!IsValid(HitActor))
	{
		return;
	}

	if(HitActors.Contains(HitActor))
	{
		return;
	}
	FPointDamageEvent DamageEvent;
	DamageEvent.HitInfo = HitResult;
	DamageEvent.ShotDirection = Direction;
	DamageEvent.DamageTypeClass = CurrentAttack->DamageTypeClass;

	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	AController* Controller = IsValid(CharacterOwner) ? CharacterOwner->GetController() : nullptr;
	HitActor->TakeDamage(CurrentAttack->DamageAmount,DamageEvent, Controller, GetOwner());

	HitActors.Add(HitActor);

}

void AMeleeWeaponItem::OnAttackTimerElapsed()
{
	CurrentAttack = nullptr;
	SetIsHitRegistrationEnabled(false);
}
