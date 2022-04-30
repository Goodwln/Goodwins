// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacterAnimInstance.h"
#include "../BaseCharacter.h"
#include "../BaseCharacterMovementComponent.h"

void UBaseCharacterAnimInstance::NativeBeginPlay()
{
	Super::NativeBeginPlay();
	checkf(TryGetPawnOwner()->IsA<ABaseCharacter>(), TEXT("UBaseCharacterAnimInstance::NativeBeginPlay() UBaseCharacterAnimInstance can be used  only with ABaseCharacter "));
	CachedBaseCharacter = Cast<ABaseCharacter>(TryGetPawnOwner());
}

void UBaseCharacterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (!CachedBaseCharacter.IsValid())
	{
		return;
	}

	UBaseCharacterMovementComponent* CharacterMovement = CachedBaseCharacter->GetBaseCharacterMovementComponent();
	Speed = CharacterMovement->Velocity.Size();
	bIsFalling = CharacterMovement->IsFalling();
	bIsCrouching = CharacterMovement->IsCrouching();
	bIsJumping = CharacterMovement->IsFalling();
	bIsSprinting = CharacterMovement->IsSprinting();
	bIsOutStamina = CharacterMovement->GetIsOutOfStamina();
	bIsProne = CharacterMovement->IsProne();
	bIsLadder = CharacterMovement->IsOnLadder();
	bIsSwimming = CharacterMovement->IsSwimming();
	bIsZipLine = CharacterMovement->IsOnZipline();
	bIsWallRun = CharacterMovement->IsOnWallRun();
	CurrentSide = CharacterMovement->GetCurrentWallRunSide();
	bIsSlide = CharacterMovement->IsOnSlide();
	bIsSuperSlide = CharacterMovement->GetIsSlideBeginningOfTheEnd();
	
	Direction = CalculateDirection(CharacterMovement->Velocity, CachedBaseCharacter->GetActorRotation());
	bIsStrafe = !CharacterMovement->bOrientRotationToMovement;

	const UCharacterEquipmentComponent* CharacterEquipment = CachedBaseCharacter->GetCharacterEquipmentComponent();
	CurrentEquippedItemType = CharacterEquipment->GetCurrentEquippedItemType();
	AimRotation = CachedBaseCharacter->GetAimOffset();

	ARangeWeaponItem* CurrentRangeWeaponItem = CharacterEquipment->GetCurrentRangeWeapon();
	if(IsValid(CurrentRangeWeaponItem))
	{
		ForeGripSocketTransform = CurrentRangeWeaponItem->GetForeGripTransform();
	}
	
	if (bIsLadder)
	{
		LadderSpeedRatio = CharacterMovement->GetLadderSpeedRatio();
	}

	bIsAiming = CachedBaseCharacter->GetIsAiming();
	
} 