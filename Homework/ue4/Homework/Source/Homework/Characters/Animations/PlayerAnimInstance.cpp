// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerAnimInstance.h"
#include "../PlayerCharacter.h"

void UPlayerAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<APlayerCharacter>(), TEXT("UPlayerAnimInstance can be used with Player Character Only"));
	CachedPlayerCharacter = StaticCast<APlayerCharacter*>(TryGetPawnOwner());
}

void UPlayerAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CachedPlayerCharacter.IsValid())
	{
		return;
	}

	IKLeftFootEffectorLocation = FVector(CachedPlayerCharacter->GetIKLeftFootOffset() , 0.f, 0.f);
	IKRightFootEffectorLocation = FVector(CachedPlayerCharacter->GetIKRightFootOffset() * -1, 0.f, 0.f);
	
	IKHitOffset = CachedPlayerCharacter->GetIKHipOffset();
}