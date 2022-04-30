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

	bNetLoadOnClient = true;
	bAlwaysRelevant = true;
	SetReplicates(true);
	
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

bool ARangeWeaponItem::IsFiring() const
{
	return bIsFiring;
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
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if(!IsValid(CharacterOwner))
	{
		return;
	}
	
	
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

	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if(!bIsSuccess)
	{
		if(IsValid(CharacterOwner))
		{
			CharacterOwner->StopAnimMontage(CurrentWeaponModeProperty.CharacterReloadMontage);
		}
		
		StopAnimMontage(CurrentWeaponModeProperty.WeaponReloadMontage);
		return;
	}

	if(CurrentWeaponModeProperty.ReloadMode == EReloadMode::PerBullet)
	{
		UAnimInstance* AnimInstance = IsValid(CharacterOwner) ? CharacterOwner->GetMesh()->GetAnimInstance() : nullptr;
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


bool ARangeWeaponItem::IsReloading() const
{
	return bIsReloading;
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

void ARangeWeaponItem::OnRep_CurrentWeaponMode(EWeaponMode CurrentWeaponMode_Old)
{
	if(CurrentWeaponMode_Old == CurrentWeaponMode)
	{
		SecondaryFire();
	}
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

void ARangeWeaponItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	 DOREPLIFETIME(ARangeWeaponItem, CurrentWeaponModeProperty);
	 DOREPLIFETIME(ARangeWeaponItem, CurrentWeaponMode);

}

void ARangeWeaponItem::Server_SecondaryFire_Implementation()
{
	SecondaryFire();
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
	ABaseCharacter* CharacterOwner = GetCharacterOwner();
	if(!IsValid(CharacterOwner))
	{
		return; 
	}

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

	FVector ShotLocation;
	FRotator ShotRotation;
	if(CharacterOwner->IsPlayerControlled())
	{
		APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
		Controller->GetPlayerViewPoint(ShotLocation, ShotRotation);
	}
	else
	{
		ShotLocation = WeaponBarell->GetComponentLocation();
		ShotRotation = CharacterOwner->GetBaseAimRotation();
	}
	
	FVector ShotDirection = ShotRotation.RotateVector(FVector::ForwardVector);
	
	SetAmmo(CurrentWeaponModeProperty.CurrentAmmo - 1);

	WeaponBarell->Shot(ShotLocation,ShotDirection, GetCurrentBulletSpreadAngle());
	
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
