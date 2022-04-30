// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseCharacter.h"

#include "AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "BaseCharacterMovementComponent.h"
#include "Actors/Environment/PlatformTrigger.h"
#include "Actors/Interactive/InteractiveActor.h"
#include "Actors/Interactive/Enviroment/Ladder.h"
#include "Actors/Interactive/Enviroment/Zipline.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"
#include "Components/CharacterComponents/CharacterEquipmentComponent.h"
#include "Components/DetectorComponents/LedgeDetectorComponent.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "Curves/CurveVector.h"
#include "GameFramework/PhysicsVolume.h"
#include "Homework/HomeworkTypes.h"
#include "Homework/Components/DetectorComponents/FloorDetectorComponent.h"
#include "Homework/Components/DetectorComponents/WallDetectorComponent.h"
 


ABaseCharacter::ABaseCharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer.SetDefaultSubobjectClass<UBaseCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{	
	BaseCharacterMovementComponent = StaticCast<UBaseCharacterMovementComponent*>(GetCharacterMovement());

	LedgeDetectorComponent = CreateDefaultSubobject<ULedgeDetectorComponent>(TEXT("LedgeDetector"));

	WallDetectorComponent = CreateDefaultSubobject<UWallDetectorComponent>(TEXT("WallDetector"));

	FloorDetectorComponent = CreateDefaultSubobject<UFloorDetectorComponent>(TEXT("FloorDetector"));

	CharacterEquipmentComponent = CreateDefaultSubobject<UCharacterEquipmentComponent>(TEXT("CharacterEquipment"));
	
	CharacterAttributeComponent = CreateDefaultSubobject<UCharacterAttributeComponent>(TEXT("CharacterAttributes"));

	
}

void ABaseCharacter::ChangeCrouchState()
{

	if (GetCharacterMovement()->IsCrouching())
	{
		UnCrouch();
	}
	else if(GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		UnCrouch();
	}
	else if (!GetCharacterMovement()->IsCrouching())
	{
		if(GetBaseCharacterMovementComponent()->IsOnSlide())
		{
			return;
		}
		if (BaseCharacterMovementComponent->IsProne())
		{

			UnProne(false);
		}
		Crouch();
	}
}

void ABaseCharacter::ChangeProneState()
{

	if (BaseCharacterMovementComponent->IsProne())
	{
		UnProne(true);
	}
	else if(GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		UnProne(true);
	}
	else if (!BaseCharacterMovementComponent->IsProne())
	{
		Prone();
	}
}

void ABaseCharacter::StartSprint()
{
	if(GetBaseCharacterMovementComponent()->IsOnZipline() || GetBaseCharacterMovementComponent()->IsFalling() || GetBaseCharacterMovementComponent()->IsMantling() || GetBaseCharacterMovementComponent()->IsOnSlide())
	{
		return;
	}

	if (bIsCrouched)
	{
		UnCrouch();
	}

	if (bIsProne)
	{
		UnProne(true);
	}
	CharacterAttributeComponent->SetIsActiveStamina(true);
	bIsSprintRequested = true;
	OnSprintStart();
}

void ABaseCharacter::StopSprint()
{
	CharacterAttributeComponent->SetIsActiveStamina(false);
	bIsSprintRequested = false;
	OnSprintEnd();
}

void ABaseCharacter::StartProne(float HeightAdjust, float ScaledHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HeightAdjust + BaseCharacterMovementComponent->CrouchedHalfHeight;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HeightAdjust + BaseCharacterMovementComponent->CrouchedHalfHeight;
	}
	OnProneStart(HeightAdjust, ScaledHeightAdjust);
}

void ABaseCharacter::EndProne(float HeightAdjust, float ScaledHeightAdjust)
{
	RecalculateBaseEyeHeight();

	const ACharacter* DefaultChar = GetDefault<ACharacter>(GetClass());
	const float HeightDifference = BaseCharacterMovementComponent->bIsFullHeight ?
		0.f : BaseCharacterMovementComponent->CrouchedHalfHeight - BaseCharacterMovementComponent->GetProneCapsuleHalfHeight();
	if (GetMesh() && DefaultChar->GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = DefaultChar->GetMesh()->GetRelativeLocation().Z + HeightDifference;
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	else
	{
		BaseTranslationOffset.Z = DefaultChar->GetBaseTranslationOffset().Z + HeightDifference;
	}

	OnProneEnd(HeightAdjust, ScaledHeightAdjust);
}

void ABaseCharacter::Jump()
{
	if (!GetBaseCharacterMovementComponent()->IsProne())
	{
		Super::Jump();
	}
	else
	{
		UnProne(true);
	}

	if (!GetBaseCharacterMovementComponent()->IsOnWallRun())
	{
		Super::Jump();
	}
	else
	{
		GetBaseCharacterMovementComponent()->bWallRunJump = true;
	}
}

bool ABaseCharacter::IsSwimmingUnderWater()
{
	APhysicsVolume* Volume = GetCharacterMovement()->GetPhysicsVolume();
	if(Volume)
	{
		float TopVolume = Volume->GetActorLocation().Z + Volume->GetBounds().BoxExtent.Z;
		FVector HeadLocation = GetMesh()->GetSocketLocation(TEXT("head"));
		
		float Offset = 20.f;
		if (TopVolume - Offset < HeadLocation.Z)
		{
			GetWorld()->GetTimerManager().ClearTimer(TimerPerDamage);
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}

void ABaseCharacter::TryChangeOxygenState(float DeltaSeconds)
{
	if(CharacterAttributeComponent->GetIsActiveOxygen() && !GetBaseCharacterMovementComponent()->IsSwimming())
	{
		GetWorld()->GetTimerManager().ClearTimer(TimerPerDamage);
		CharacterAttributeComponent->SetIsActiveOxygen(false);
	}
	
	if(!GetBaseCharacterMovementComponent()->IsSwimming())
	{
		return;
	}
	CharacterAttributeComponent->SetIsActiveOxygen(IsSwimmingUnderWater());
	
}

void ABaseCharacter::Tick(float DeltaSeconds) // 7
{
	Super::Tick(DeltaSeconds);
	
	TryChangeSprintState(DeltaSeconds);

	TryChangeOxygenState(DeltaSeconds);
}

void ABaseCharacter::BeginPlay()
{
	Super::BeginPlay();

	CharacterAttributeComponent->OnDeathEvent.AddUObject(this, &ABaseCharacter::OnDeath);
	CharacterAttributeComponent->OnChangeStaminaEvent.AddUObject(this, &ABaseCharacter::OnChangeStamina);
	CharacterAttributeComponent->OnChangeOxygenEvent.AddUObject(this, &ABaseCharacter::OnChangeOxygen);
	CharacterAttributeComponent->OnChangeHealthEvent.AddUObject(this, &ABaseCharacter::OnChangeHealth);
 
}	

void ABaseCharacter::Mantle(bool bForce /*=false*/)
{

	if(!(CanMantle() ||	bForce))
	{	
		return;
	}				
	
	if (GetBaseCharacterMovementComponent()->IsProne())
	{
		return;
	}
	
	FLedgeDescription LedgeDescription;
	if (LedgeDetectorComponent->DetectLedge(LedgeDescription) && !GetBaseCharacterMovementComponent()->IsMantling()) 
	{
		
		if (GetBaseCharacterMovementComponent()->IsCrouching())
		{
			UnCrouch();
		}

		bIsMantling = true;
		
		FMantlingMovementParameters MantlingParametrs;

		MantlingParametrs.InitialLocation = GetActorLocation();
		MantlingParametrs.InitialRotation = GetActorRotation();
		MantlingParametrs.TargetLocation = LedgeDescription.Location;
		MantlingParametrs.TargetRotation = LedgeDescription.Rotation;
		MantlingParametrs.ActorTargetLedge = LedgeDescription.ActorLedge;
		MantlingParametrs.SaveLocation = LedgeDescription.ActorLedge->GetComponentLocation();
		MantlingParametrs.MantlingOffset = MantlingParametrs.TargetLocation - MantlingParametrs.SaveLocation;
		const float MantlingHeight = LedgeDescription.MantlingHeight; 
		const FMantlingSettings& MantlingSettings = GetMantlingSettings(MantlingHeight); 
		float MinRange;
		float MaxRange;
		MantlingSettings.MantlingCurveVector->GetTimeRange(MinRange, MaxRange);
		
		MantlingParametrs.Duration = MaxRange - MinRange;
		MantlingParametrs.MantlingCurve = MantlingSettings.MantlingCurveVector;
		
		const FVector2D SourceRange(MantlingSettings.MinHeight, MantlingSettings.MaxHeight);
		const FVector2D TargetRange(MantlingSettings.MinHeightStartTime, MantlingSettings.MaxHeightStartTime);
		MantlingParametrs.StartTime = FMath::GetMappedRangeValueClamped(SourceRange, TargetRange, MantlingHeight);
 
		MantlingParametrs.InitialAnimationLocation = MantlingParametrs.TargetLocation - MantlingSettings.AnimationCorectionZ * FVector::UpVector + MantlingSettings.AnimationCorectionXY * LedgeDescription.LedgeNormal;
		if(GetLocalRole() > ROLE_SimulatedProxy)
		{
			GetBaseCharacterMovementComponent()->StartMantle(MantlingParametrs);
		}
		
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		AnimInstance->Montage_Play(MantlingSettings.MantlingMontage, 1.f, EMontagePlayReturnType::Duration, MantlingParametrs.StartTime);
	}	
}

void ABaseCharacter::SetPlayerColor(const FLinearColor& LinearColor) 
{
	UMaterialInstanceDynamic* MaterialInstance = GetMesh()->CreateAndSetMaterialInstanceDynamic(0);
	if(!IsValid(MaterialInstance))
	{
		return;
	}

	MaterialInstance->SetVectorParameterValue(MaterialColorName, LinearColor);
}

void ABaseCharacter::OnRep_IsMantling(bool bWasMantling)
{
	if(GetLocalRole() == ROLE_SimulatedProxy && (!bWasMantling && bIsMantling))
	{
		Mantle(true);
	}
}

void ABaseCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABaseCharacter, bIsMantling);
}

void ABaseCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if(IsValid(AIController))
	{
		FGenericTeamId TeamId((uint8)Team);
		AIController->SetGenericTeamId(TeamId);
	}
}

bool ABaseCharacter::CanJumpInternal_Implementation() const
{
	return Super::CanJumpInternal_Implementation() && !GetBaseCharacterMovementComponent()->IsMantling();
}


bool ABaseCharacter::CanProne() const
{
	return !bIsProne && GetBaseCharacterMovementComponent() && GetRootComponent() && !GetRootComponent()->IsSimulatingPhysics();
}

void ABaseCharacter::RegisterInteractiveActor(class AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.Add(InteractiveActor);
}

void ABaseCharacter::UnRegisterInteractiveActor(class AInteractiveActor* InteractiveActor)
{
	AvailableInteractiveActors.RemoveSingleSwap(InteractiveActor);
}

void ABaseCharacter::Prone()
{
	if (GetBaseCharacterMovementComponent())
	{
		if (CanProne())
		{
			GetBaseCharacterMovementComponent()->bWantsToProne = true;
		}
	}
}

void ABaseCharacter::UnProne(bool bIsFullHeight)
{
	if (GetBaseCharacterMovementComponent())
	{
		GetBaseCharacterMovementComponent()->bWantsToProne = false;
		GetBaseCharacterMovementComponent()->bIsFullHeight = bIsFullHeight;
	}
}

bool ABaseCharacter::CanMantle() const
{
	return !GetBaseCharacterMovementComponent()->IsOnLadder() && !GetBaseCharacterMovementComponent()->IsOnZipline();
}

void ABaseCharacter::RagdollEnable()
{
	GetMesh()->SetCollisionProfileName(CollisionProfileRagdoll);
	GetMesh()->SetSimulatePhysics(true);
}

void ABaseCharacter::OnDeath()
{
	GetCharacterEquipmentComponent_Mutable()->UnEquipCurrentItem();
	GetCharacterEquipmentComponent_Mutable()->ClearEquip();
	float Duration = PlayAnimMontage(OnDeathAnimMontage);                
	GetCharacterMovement()->DisableMovement();
  	GetCapsuleComponent()->SetActive(false);
	GetCapsuleComponent()->SetSimulatePhysics(false);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	if(Duration == 0)
	{
		RagdollEnable();
	}
}

void ABaseCharacter::OnStartAimingInternal()
{
	if(OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(true);
	}
}

void ABaseCharacter::OnStopAimingInternal()
{
	if(OnAimingStateChanged.IsBound())
	{
		OnAimingStateChanged.Broadcast(false);
	}
}

void ABaseCharacter::Reset()
{
	GetCharacterEquipmentComponent_Mutable()->UnEquipCurrentItem();
	GetCharacterEquipmentComponent_Mutable()->ClearEquip();
	Super::Reset();
	
}

void ABaseCharacter::TryChangeSprintState(float DeltaSeconds)
{
	if (!GetBaseCharacterMovementComponent()->GetIsOutOfStamina() && bIsSprintRequested && !GetBaseCharacterMovementComponent()->IsSprinting())
	{
		GetBaseCharacterMovementComponent()->StartSprint();
		OnSprintStart();
	}

	if (!bIsSprintRequested && GetBaseCharacterMovementComponent()->IsSprinting())
	{
		GetBaseCharacterMovementComponent()->StopSprint();
		OnSprintEnd();
	}
}

void ABaseCharacter::PerDamage()
{
	if(!CharacterAttributeComponent->GetIsActiveOxygen())
	{
		return;
	}
	
	CharacterAttributeComponent->TakeDamage(GetOwner(),1.f, GetDefault<UDamageType>(), GetController(), GetOwner());
}

void ABaseCharacter::OnChangeOxygen(float Current, float Max, FDefaultAttributeProperty Attribute)
{ 
	if(Current <= 0.f)
	{
		GetWorld()->GetTimerManager().SetTimer(TimerPerDamage,this,&ABaseCharacter::PerDamage, 1.f, true);
	} 
}

void ABaseCharacter::OnChangeStamina(float Current, float Max, FDefaultAttributeProperty Attribute)
{
	if(Current <= 0.f)
	{
		GetBaseCharacterMovementComponent()->SetIsOutOfStamina(true);
		GetBaseCharacterMovementComponent()->StopSprint();
		return; 
	}
	
	if(!GetBaseCharacterMovementComponent()->GetIsOutOfStamina())
	{
		return;
	}

	if(Current == Max)
	{
		GetBaseCharacterMovementComponent()->SetIsOutOfStamina(false);
	} 	
}

void ABaseCharacter::OnChangeHealth(float Current, float Max, FDefaultAttributeProperty Attribute)
{
}

const FMantlingSettings& ABaseCharacter::GetMantlingSettings(float LedgeHeight) const
{
	if (GetBaseCharacterMovementComponent()->IsSwimming())
	{
		return HeightMantlingSettings;
	}

	if (GetBaseCharacterMovementComponent()->IsCrouching())
	{
		return LedgeHeight > LowMantleMaxHeight ? HeightMantlingSettingsFromCrouch : LowMantlingSettingsFromCrouch;
	}

	return LedgeHeight > LowMantleMaxHeight ? HeightMantlingSettings : LowMantlingSettings;
}

void ABaseCharacter::Falling()
{
	Super::Falling();

	GetBaseCharacterMovementComponent()->bNotifyApex = true;
}

void ABaseCharacter::NotifyJumpApex()
{
	Super::NotifyJumpApex();
	CurrentFallApex = GetActorLocation();

}

void ABaseCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	float FallHeight = ( CurrentFallApex - GetActorLocation()).Z;

	UE_LOG(LogTemp, Warning, TEXT("FallHeight = %f"), FallHeight / 100);
	if(IsValid(FallDamageCurve))
	{
		float DamageAmount = FallDamageCurve->GetFloatValue(FallHeight / 100);
		TakeDamage(DamageAmount, FDamageEvent(), GetController(), Hit.Actor.Get());
	}
}

  FGenericTeamId ABaseCharacter::GetGenericTeamId() const
 {
	return FGenericTeamId((uint8)Team);
 }

bool ABaseCharacter::IsAiming()
{
	return bIsAiming;
}

float ABaseCharacter::GetCurrentAimingMovementSpeed()
{
	return CurrentAimingMovementSpeed;
}

void ABaseCharacter::ClimbLadderUp(float Value)
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder() && !FMath::IsNearlyZero(Value))
	{
		const FVector LadderUpVector = GetBaseCharacterMovementComponent()->GetCurrentLadder()->GetActorUpVector();
		AddMovementInput(LadderUpVector, Value);
	}
}

void ABaseCharacter::InteractWithZipline()
{
	if (GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		GetBaseCharacterMovementComponent()->DetachFromZipline();
	}
	else
	{
		const AZipline* AvailableZipline = GetAvailableZipline();
		if (IsValid(AvailableZipline))
		{
			GetBaseCharacterMovementComponent()->AttachToZipline(AvailableZipline);
		}
	}
}

void ABaseCharacter::StartSlide()
{
	if(GetBaseCharacterMovementComponent()->IsMantling() || GetBaseCharacterMovementComponent()->IsProne()  || GetBaseCharacterMovementComponent()->IsOnSlide() || GetBaseCharacterMovementComponent()->IsCrouching())
	{
		return;
	}

	if(!GetBaseCharacterMovementComponent()->IsSprinting() || GetBaseCharacterMovementComponent()->GetIsOutOfStamina())
	{
		return;
	}
	
	StopSprint();
	bUseControllerRotationYaw = false;
	
	FFloorProperty FloorProperty;
	if(FloorDetectorComponent->DetectFloor(FloorProperty))
	{
		OnStartSlide();
		FSlideParameters SlideParametrs;
		SlideParametrs.Angle = FloorProperty.Angle;
		GetBaseCharacterMovementComponent()->StartSlide(SlideParametrs);
	}
}

void ABaseCharacter::StopSlide()
{
	bUseControllerRotationYaw = true;
	GetBaseCharacterMovementComponent()->SetIsSlideBeginningOfTheEnd(true);
}

void ABaseCharacter::OnStartSlide()
{
	
	if (GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = GetMesh()->GetRelativeLocation().Z + (GetBaseCharacterMovementComponent()->GetCurrentCapsuleSize(ECurrentCapsuleSize::DefaultCapsule).HalfHeight - GetBaseCharacterMovementComponent()->GetCurrentCapsuleSize(ECurrentCapsuleSize::SlideCapsule).HalfHeight);
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
}

void ABaseCharacter::OnEndSlide()
{
	
	if (GetMesh())
	{
		FVector& MeshRelativeLocation = GetMesh()->GetRelativeLocation_DirectMutable();
		MeshRelativeLocation.Z = GetMesh()->GetRelativeLocation().Z - (GetBaseCharacterMovementComponent()->GetCurrentCapsuleSize(ECurrentCapsuleSize::DefaultCapsule).HalfHeight - GetBaseCharacterMovementComponent()->GetCurrentCapsuleSize(ECurrentCapsuleSize::SlideCapsule).HalfHeight);
		BaseTranslationOffset.Z = MeshRelativeLocation.Z;
	}
	
	GetBaseCharacterMovementComponent()->StopSlide();
}

UCharacterEquipmentComponent* ABaseCharacter::GetCharacterEquipmentComponent_Mutable() const
{
	return CharacterEquipmentComponent;
}

const UCharacterEquipmentComponent* ABaseCharacter::GetCharacterEquipmentComponent() const
{
	return CharacterEquipmentComponent;
}

void ABaseCharacter::StartFire()
{
	if(GetCharacterEquipmentComponent()->IsEquipping() || GetBaseCharacterMovementComponent()->IsMantling() || GetBaseCharacterMovementComponent()->IsSprinting() || GetBaseCharacterMovementComponent()->IsOnLadder() || GetBaseCharacterMovementComponent()->IsOnSlide() || GetBaseCharacterMovementComponent()->IsOnZipline() || !GetCharacterAttributeComponent()->IsAlive())
	{
		return;
	}
	
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if(IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StartFire();
	}
}

void ABaseCharacter::StopFire()
{
	ARangeWeaponItem* CurrentRangeWeapon = CharacterEquipmentComponent->GetCurrentRangeWeapon();
	if(IsValid(CurrentRangeWeapon))
	{
		CurrentRangeWeapon->StopFire();
	}
}

void ABaseCharacter::StartAiming()
{
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if(!IsValid(CurrentRangeWeapon))
	{
		return;
	}
	
	bIsAiming = true;
	CurrentRangeWeapon->StartAim();
	CurrentAimingMovementSpeed = CurrentRangeWeapon->GetAimMovementSpeed();
	OnStartAiming();
}

void ABaseCharacter::StopAiming()
{
	if(!bIsAiming)
	{
		return;
	}
	
	ARangeWeaponItem* CurrentRangeWeapon = GetCharacterEquipmentComponent()->GetCurrentRangeWeapon();
	if(!IsValid(CurrentRangeWeapon))
	{
		return;
	}
	bIsAiming = false;
	CurrentRangeWeapon->StopAim();
	CurrentAimingMovementSpeed = 0.f;
	OnStopAiming();
}

FRotator ABaseCharacter::GetAimOffset()
{
	FVector AimDirectionWorld = GetBaseAimRotation().Vector();
	FVector AimDirectionLocal = GetTransform().InverseTransformVectorNoScale(AimDirectionWorld);
	FRotator Result = AimDirectionLocal.ToOrientationRotator();
	return Result;
}

void ABaseCharacter::OnStartAiming_Implementation()
{
	OnStartAimingInternal();
}

void ABaseCharacter::OnStopAiming_Implementation()
{
	OnStopAimingInternal();
}

bool ABaseCharacter::GetIsAiming() const
{
	return bIsAiming;
}

void ABaseCharacter::Reload()
{
	if(!GetCharacterAttributeComponent()->IsAlive())
	{
		return;
	}
	
	if(IsValid(GetCharacterEquipmentComponent()->GetCurrentRangeWeapon()))
	{
		GetCharacterEquipmentComponent_Mutable()->ReloadCurrentWeapon();
	}
	
}

void ABaseCharacter::NextItem()
{
	if(!GetCharacterAttributeComponent()->IsAlive())
	{
		return;
	}
	CharacterEquipmentComponent->EquipNextItem();
}

void ABaseCharacter::PreviousItem()
{
	if(!GetCharacterAttributeComponent()->IsAlive())
	{
		return;
	}
	CharacterEquipmentComponent->EquipPreviousItem();
}

void ABaseCharacter::EquipPrimaryItem()
{
	if(!GetCharacterAttributeComponent()->IsAlive())
	{
		return;
	}
	
	if(CharacterEquipmentComponent->IsAvailableThrowable())                 
	{
		CharacterEquipmentComponent->EquipItemInSlot(EEquipmentSlot::PrimaryItemSlot);
	}
}

void ABaseCharacter::SecondaryFire()
{
	if(!GetCharacterAttributeComponent()->IsAlive())
	{
		return;
	}
	
	if(GetCharacterEquipmentComponent()->IsEquipping() || GetBaseCharacterMovementComponent()->IsMantling() || GetBaseCharacterMovementComponent()->IsSprinting() || GetBaseCharacterMovementComponent()->IsOnLadder() || GetBaseCharacterMovementComponent()->IsOnSlide() || GetBaseCharacterMovementComponent()->IsOnZipline())
	{
		return;
	}
	
	if(IsValid(CharacterEquipmentComponent) && IsValid(CharacterEquipmentComponent->GetCurrentRangeWeapon()))
	{
		ARangeWeaponItem* WeaponItem = CharacterEquipmentComponent->GetCurrentRangeWeapon();

		WeaponItem->Server_SecondaryFire();
		
	}
}

void ABaseCharacter::PrimaryMeleeAttack()
{
	if(!GetCharacterAttributeComponent()->IsAlive())
	{
		return;
	}
	
	AMeleeWeaponItem* CurrentMeleeWeaponItem = CharacterEquipmentComponent->GetCurrentMeleeWeaponItem();
	if(IsValid(CurrentMeleeWeaponItem))
	{
		CurrentMeleeWeaponItem->StartAttack(EMeleeAttackType::PrimaryAttack);
	}
}

void ABaseCharacter::SecondaryMeleeAttack()
{
	if(!GetCharacterAttributeComponent()->IsAlive())
	{
		return;
	}
	
	AMeleeWeaponItem* CurrentMeleeWeaponItem = CharacterEquipmentComponent->GetCurrentMeleeWeaponItem();
	if(IsValid(CurrentMeleeWeaponItem))
	{
		CurrentMeleeWeaponItem->StartAttack(EMeleeAttackType::SecondaryAttack);
	}
}
 

void ABaseCharacter::InteractWithLadder()
{
	if (GetBaseCharacterMovementComponent()->IsOnLadder())
	{
		GetBaseCharacterMovementComponent()->DetachFromLadder(EDetachFromLadderMethod::JumpOff);
	}
	else
	{
		const ALadder* AvailableLadder = GetAvailableLadder();
		if (IsValid(AvailableLadder))
		{
			if(AvailableLadder->GetIsOnTop())
			{
				PlayAnimMontage(AvailableLadder->GetAttachFromTopAnimMontage());
			}
			GetBaseCharacterMovementComponent()->AttachToLadder(AvailableLadder);
		}
	}
}

const AZipline* ABaseCharacter::GetAvailableZipline() const
{
	const AZipline* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<AZipline>())
		{
			Result = StaticCast<const AZipline*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

UFloorDetectorComponent* ABaseCharacter::GetFloorDetectorComponent()
{
	return FloorDetectorComponent;
}

UCharacterAttributeComponent* ABaseCharacter::GetCharacterAttributeComponent() const
{
	return CharacterAttributeComponent;
}

ETeams ABaseCharacter::GetTeam() const
{
	return Team;
}

const ALadder* ABaseCharacter::GetAvailableLadder() const
{
	const ALadder* Result = nullptr;
	for (const AInteractiveActor* InteractiveActor : AvailableInteractiveActors)
	{
		if (InteractiveActor->IsA<ALadder>())
		{
			Result = StaticCast<const ALadder*>(InteractiveActor);
			break;
		}
	}
	return Result;
}

