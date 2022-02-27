// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_EndSlideToCrouch.h"

#include "Homework/Characters/BaseCharacter.h"
#include "Homework/Characters/BaseCharacterMovementComponent.h"

void UAnimNotify_EndSlideToCrouch::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);
	ABaseCharacter* CachedBaseCharacter = Cast<ABaseCharacter>(MeshComp->GetOwner());
	if(IsValid(CachedBaseCharacter) && CachedBaseCharacter->GetBaseCharacterMovementComponent()->GetIsEndSlideChangeOnCrouch())
	{
		CachedBaseCharacter->OnEndSlide();
	}
}
