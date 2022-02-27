// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EndSlide.h"

#include "Homework/Characters/BaseCharacter.h"
#include "Homework/Characters/BaseCharacterMovementComponent.h"

void UAnimNotify_EndSlide::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	ABaseCharacter* CacheBaseCharacter = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if(IsValid(CacheBaseCharacter))
	{
		CacheBaseCharacter->OnEndSlide();
	}
}
