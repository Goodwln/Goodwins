// Fill out your copyright notice in the Description page of Project Settings.


#include "RangeWeaponItem.h"

#include "HomeworkTypes.h"
#include "Characters/BaseCharacter.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "Kismet/KismetSystemLibrary.h"

ARangeWeaponItem::ARangeWeaponItem()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("WeaponRoot"));

	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMesh->SetupAttachment(RootComponent);

	WeaponBarell = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarell"));
	WeaponBarell->SetupAttachment(WeaponMesh,SocketWeaponMuzzle);
	
	CurrentReticle = ECurrentReticle::Default;
	
	EquippedSocketName = SocketCharacterWeapon;
}

void ARangeWeaponItem::StartFire()
{
	if(GetWorld()->GetTimerManager().IsTimerActive(ShotTimer))
	{
		return;
	}
	bIsFiring = true;	
	MakeShot();
}

void ARangeWeaponItem::StopFire()  
{
	if(CurrentWeaponModeProperty.WeaponFireMode == EWeaponFireMode::FullAuto)
	{
		GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
		bIsFiring = false;
	}
}

void ARangeWeaponItem::StartAim()
{
	bIsAiming = true;
}

void ARangeWeaponItem::StopAim()
{
	bIsAiming = false;
}

void ARangeWeaponItem::StartReload()
{
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("ARangeWeaponItem::StartReload() only character can be an owner of range weapon"));
	ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
	
	
	bIsReloading = true;
	if(IsValid(CurrentWeaponModeProperty.CharacterReloadMontage))
	{
		float AnimDuration = CharacterOwner->PlayAnimMontage(CurrentWeaponModeProperty.CharacterReloadMontage);
		PlayAnimMontage(CurrentWeaponModeProperty.WeaponReloadMontage);

		if(CurrentWeaponModeProperty.ReloadMode == EReloadMode::FullClip)
		{
			GetWorld()->GetTimerManager().SetTimer(ReloadTimer,[this](){ EndReload(true); }, AnimDuration, false );
		}
	}
	else
	{
		EndReload(true);	
	}
}

void ARangeWeaponItem::EndReload(bool bIsSuccess)
{
	if(!bIsReloading)
	{
		return;
	}

	if(!bIsSuccess)
	{
		checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("ARangeWeaponItem::StartReload() only character can be an owner of range weapon"));
		ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
		CharacterOwner->StopAnimMontage(CurrentWeaponModeProperty.CharacterReloadMontage);
		StopAnimMontage(CurrentWeaponModeProperty.WeaponReloadMontage);
		return;
	}

	if(CurrentWeaponModeProperty.ReloadMode == EReloadMode::PerBullet)
	{
		ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
		UAnimInstance* AnimInstance = CharacterOwner->GetMesh()->GetAnimInstance();
		if(IsValid(AnimInstance))
		{
			AnimInstance->Montage_JumpToSection(SectionMontageReloadName, CurrentWeaponModeProperty.CharacterReloadMontage);
		}

		UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
		if(IsValid(WeaponAnimInstance))
		{
			WeaponAnimInstance->Montage_JumpToSection(SectionMontageReloadName, CurrentWeaponModeProperty.WeaponReloadMontage);
		}
	}
	
	GetWorld()->GetTimerManager().ClearTimer(ReloadTimer);
	bIsReloading = false;
	if(OnReloadComplete.IsBound())
	{
		OnReloadComplete.Broadcast();
	}
}

float ARangeWeaponItem::GetAimFOV()
{
	return AimFOV;
}

UCurveFloat* ARangeWeaponItem::GetFOVCurve()
{
	return FOVCurve;
}

float ARangeWeaponItem::GetAimMovementSpeed() const
{
	return AimMovementSpeed;	
}

FTransform ARangeWeaponItem::GetForeGripTransform() const
{
	return WeaponMesh->GetSocketTransform(SocketWeaponForeGrip);
}

float ARangeWeaponItem::GetAimTurnModifier() const
{
	
	return AimTurnModifier;
}

float ARangeWeaponItem::GetAimLookUpModifier() const
{
	return AimLookUpModifier;
}

int32 ARangeWeaponItem::GetMaxAmmo() const
{
	return CurrentWeaponModeProperty.AmmoMax;
}

int32 ARangeWeaponItem::GetAmmo() const
{
	return CurrentWeaponModeProperty.CurrentAmmo;
}

void ARangeWeaponItem::SetAmmo(int NewAmmo)
{
	CurrentWeaponModeProperty.CurrentAmmo = NewAmmo;
	if(OnAmmoChange.IsBound())
	{
		OnAmmoChange.Broadcast(CurrentWeaponModeProperty.CurrentAmmo);
	}
}

bool ARangeWeaponItem::CanShoot() const
{
	return CurrentWeaponModeProperty.CurrentAmmo > 0;
}

EAmunitionType ARangeWeaponItem::GetAmunitionType() const
{
	return CurrentWeaponModeProperty.AmmoType;
}

FWeaponModeProperty ARangeWeaponItem::NextWeaponMode(bool bWasCurrentKey)
{
	for (auto& Elem : AllWeaponModeProperty)
	{
		if (CurrentWeaponMode == Elem.Key)
		{
			Elem.Value = CurrentWeaponModeProperty;
		}

		if (bWasCurrentKey){
			CurrentWeaponMode = Elem.Key;
			return Elem.Value;
		}
		
		bWasCurrentKey = Elem.Key == CurrentWeaponMode ? true : false;
	}
	
	return NextWeaponMode(bWasCurrentKey);
}

void ARangeWeaponItem::SecondaryFire()
{
	bool bWasCurrentKey = false;
	CurrentWeaponModeProperty = NextWeaponMode(bWasCurrentKey);
	SetAmmo(CurrentWeaponModeProperty.CurrentAmmo);

	WeaponBarell->ChangeWeaponBarrelProperty(CurrentWeaponMode);
}

ECurrentReticle ARangeWeaponItem::GetCurrentReticle() const
{
	return bIsAiming ? AimCurrentReticle : CurrentReticle;
}


void ARangeWeaponItem::BeginPlay()
{
	Super::BeginPlay();
	InitializeWeaponMode();
	
}

float ARangeWeaponItem::GetCurrentBulletSpreadAngle() const
{
	float AngleToDegress = bIsAiming ? CurrentWeaponModeProperty.AimSpreadAngle : CurrentWeaponModeProperty.SpreadAngle;
	return FMath::DegreesToRadians(AngleToDegress);
}

void ARangeWeaponItem::MakeShot()
{
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("ARangeWeaponItem::Fire() only character can be an owner of range weapon"));
	ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());

	if(bIsReloading)
	{
		return;
	}
	
	if(!CanShoot())
	{
		StopFire();
		if(bAutoReload)
		{
			CharacterOwner->Reload();
		}
		return;
	}
	
	CharacterOwner->PlayAnimMontage(CurrentWeaponModeProperty.CharacterFireMontage);
	PlayAnimMontage(CurrentWeaponModeProperty.WeaponFireMontage);
	
	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
	if (!IsValid(Controller))
	{
		return;
	}
	
	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;
	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);
	
	FVector Direction = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	
	SetAmmo(CurrentWeaponModeProperty.CurrentAmmo - 1);

	WeaponBarell->Shot(PlayerViewPoint,Direction, GetCurrentBulletSpreadAngle());
	
	GetWorld()->GetTimerManager().SetTimer(ShotTimer, this, &ARangeWeaponItem::OnShotTimerElapsed, GetShotTimerInterval(), false);
}

void ARangeWeaponItem::OnShotTimerElapsed()
{
	if(bAutoReload && !CanShoot())
	{
		ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
		CharacterOwner->Reload();
	}
	
	if(!bIsFiring)
	{
		return;
	}
	if(CurrentWeaponModeProperty.WeaponFireMode == EWeaponFireMode::Single)
	{
		bIsFiring = false;
		return;
	}
	
	GetWorld()->GetTimerManager().ClearTimer(ShotTimer);
	
	StartFire();
}

void ARangeWeaponItem::InitializeWeaponMode()
{
	for(auto& Elem : AllWeaponModeProperty)
	{
		Elem.Value.CurrentAmmo = Elem.Value.AmmoMax;

		if(Elem.Key == StartingWeaponMode)
		{
			CurrentWeaponModeProperty = Elem.Value;
		}
	}
	CurrentWeaponMode = StartingWeaponMode;
	WeaponBarell->ChangeWeaponBarrelProperty(CurrentWeaponMode);
}


float ARangeWeaponItem::GetShotTimerInterval() const
{
	return  60.f / CurrentWeaponModeProperty.RateOfFire;
}

float ARangeWeaponItem::PlayAnimMontage(UAnimMontage* AnimMontage)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	float Result = 0.f;
	if(IsValid(WeaponAnimInstance))
	{
		Result = WeaponAnimInstance->Montage_Play(AnimMontage);
	}
	return Result;
}

void ARangeWeaponItem::StopAnimMontage(UAnimMontage* AnimMontage, float BlendOutTime)
{
	UAnimInstance* WeaponAnimInstance = WeaponMesh->GetAnimInstance();
	if(IsValid(WeaponAnimInstance))
	{
		WeaponAnimInstance->Montage_Stop(BlendOutTime ,AnimMontage);
	}
}
