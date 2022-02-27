// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponBarellComponent.h"
#include "DrawDebugHelpers.h"
#include "HomeworkTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/DebugSubsystem.h"
#include "Utils/TraceUtils.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Components/DecalComponent.h"
#include "Actors/Projectiles/BaseProjectile.h"

void UWeaponBarellComponent::TraceFXPainting(FVector ShotEnd)
{
	FVector MuzzleStart = GetComponentLocation();
	UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CurrentWeaponBarrelProperty.MuzzleFlashFX, MuzzleStart, GetComponentRotation());

	UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
		GetWorld(), CurrentWeaponBarrelProperty.TraceFX, MuzzleStart, GetComponentRotation());
	TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);
}

void UWeaponBarellComponent::DrawDecal(FHitResult ShotResult)
{
	UDecalComponent* DecalComponent = UGameplayStatics::SpawnDecalAtLocation(
		GetWorld(), CurrentWeaponBarrelProperty.DefaultDecalInfo.Material, CurrentWeaponBarrelProperty.DefaultDecalInfo.Size, ShotResult.ImpactPoint,
		ShotResult.ImpactNormal.ToOrientationRotator());

	if (DecalComponent)
	{
		DecalComponent->SetFadeScreenSize(CurrentWeaponBarrelProperty.DefaultDecalInfo.FadeScreenSize);
		DecalComponent->SetFadeOut(CurrentWeaponBarrelProperty.DefaultDecalInfo.LifeTime, CurrentWeaponBarrelProperty.DefaultDecalInfo.FadeOutTime);
	}
}

void UWeaponBarellComponent::BeginPlay()
{
	Super::BeginPlay();
}

bool UWeaponBarellComponent::HitScan(FVector ShotStart, FVector ShotDirection, float SpreadAngle)
{
	ShotDirection += GetBulletSpreadOffset(FMath::RandRange(0.f, SpreadAngle),
										   ShotDirection.ToOrientationRotator());

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = false;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(GetOwner()->GetOwner());
	
	FVector ShotEnd = ShotStart + ShotDirection * CurrentWeaponBarrelProperty.FireRange;
	
	FHitResult TraceResult;
	TraceUtils::LineTraceSingleByChanel(GetWorld(), TraceResult, ShotStart, ShotEnd, ECC_Bullet, QueryParams,
											FCollisionResponseParams::DefaultResponseParam);
	
	if(TraceResult.ImpactPoint == FVector::ZeroVector)
	{
		TraceFXPainting(ShotEnd);
		return false;
	}
	
	ShotEnd = TraceResult.ImpactPoint;
	ShotStart = GetComponentLocation();
	
	FVector Direction = ShotEnd - ShotStart;
	TraceUtils::LineTraceSingleByChanel(GetWorld(), TraceResult, ShotStart, ShotEnd + Direction, ECC_Bullet, QueryParams,
											FCollisionResponseParams::DefaultResponseParam);
	
	if(IsValid(TraceResult.GetComponent()))
	{
		ProcessHit(TraceResult, Direction);
		TraceFXPainting(ShotEnd);
		return true;
	}

	return false;
}

void UWeaponBarellComponent::LaunchProjectile(const FVector& Direction)
{
	const FVector Start = GetComponentLocation();
	
	ABaseProjectile* Projectile = GetWorld()->SpawnActor<ABaseProjectile>(CurrentWeaponBarrelProperty.ProjectileClass, Start, Direction.ToOrientationRotator());
	if(IsValid(Projectile))
	{
		StartShot = Start;
		Projectile->SetOwner(GetPawnOwner());
		Projectile->OnProjectileHit.AddDynamic(this, &UWeaponBarellComponent::ProcessHit);
		Projectile->LaunchProjectile(Direction.GetSafeNormal());
	}
}

void UWeaponBarellComponent::ProcessHit(const FHitResult& HitResult, const FVector& Direction)
{
	FVector HitLocation = HitResult.ImpactPoint;
	AActor* HitActor = Cast<AActor>( HitResult.GetActor());
	if (IsValid(HitActor))
	{
		FPointDamageEvent DamageEvent;
		DamageEvent.HitInfo = HitResult;
		DamageEvent.ShotDirection = Direction;
		DamageEvent.DamageTypeClass = CurrentWeaponBarrelProperty.DamageTypeClass;

		if (IsValid(CurrentWeaponBarrelProperty.FalloffDiagram))
		{
			const float Damage = CurrentWeaponBarrelProperty.DamageAmount * CurrentWeaponBarrelProperty.FalloffDiagram->GetFloatValue(
				FVector::Distance(StartShot, HitLocation) / 100.f);
				
			HitActor->TakeDamage(Damage, DamageEvent, GetController(), GetOwner());
		}
		else
		{
			HitActor->TakeDamage(CurrentWeaponBarrelProperty.DamageAmount, DamageEvent, GetController(),
								 GetOwner());
		}
	}
	DrawDecal(HitResult);
}

void UWeaponBarellComponent::ProjectileScan(FVector& ShotStart, FVector& ShotDirection)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());
	QueryParams.AddIgnoredActor(GetOwner()->GetOwner());
	
	FVector ShotEnd = ShotStart + ShotDirection * CurrentWeaponBarrelProperty.FireRange;
	
	FHitResult TraceResult;
	TraceUtils::LineTraceSingleByChanel(GetWorld(), TraceResult, ShotStart, ShotEnd, ECC_Bullet, QueryParams,
	                                    FCollisionResponseParams::DefaultResponseParam);
	if(TraceResult.ImpactPoint == FVector::ZeroVector)
	{
		float Offset = 2000.f;
		ShotEnd = ShotStart + ShotDirection * Offset;
		FVector Direction = ShotEnd - ShotStart;
		LaunchProjectile(Direction);
		return;
	}
	ShotEnd = TraceResult.ImpactPoint;
	ShotStart = GetComponentLocation();
	FVector Direction = ShotEnd - ShotStart;
	LaunchProjectile(Direction);
}

void UWeaponBarellComponent::Shot(FVector& ShotStart, FVector& ShotDirection, float SpreadAngle)
{
#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnable = DebugSubsystem->IsCategoryEnable(DebugCategoryShotDetection);
#else
	bool bIsDebugEnable = false;
#endif
	float DrawTime = 2.f;

	bool bIsTrueShot = false;
	
	for (int i = 0; i < CurrentWeaponBarrelProperty.BulletPerShot; i++)
	{
		switch (CurrentWeaponBarrelProperty.HitRegistration)
		{
			case EHitRegistrationType::HitScan:
			{
				bIsTrueShot = HitScan(ShotStart, ShotDirection, SpreadAngle);
				break;
			}
			case EHitRegistrationType::Projectile:
			{
				ProjectileScan(ShotStart, ShotDirection);
				break;
			}
		}
	}
	
#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	if (bIsDebugEnable && bIsTrueShot)
	{
		DrawDebugSphere(GetWorld(), ShotDirection, 10.f, 24, FColor::Red, true, DrawTime);
	}
#endif
	
}

float UWeaponBarellComponent::GetBulletPerShot() const
{
	return CurrentWeaponBarrelProperty.BulletPerShot;
}

EHitRegistrationType UWeaponBarellComponent::GetHitRegistration() const
{
	return CurrentWeaponBarrelProperty.HitRegistration;
}

void UWeaponBarellComponent::ChangeWeaponBarrelProperty(EWeaponMode CurrentWeaponMode)
{
	for(const auto &Elem : AllWeaponBarrelProperty)
	{
		if(Elem.Key == CurrentWeaponMode)
		{
			CurrentWeaponBarrelProperty = Elem.Value;
		}
	}
}


FVector UWeaponBarellComponent::GetBulletSpreadOffset(float Angle, FRotator ShotRotation) const
{
	float SpreadSize = FMath::Tan(Angle);
	float RotationAngle = FMath::RandRange(0.f, 2 * PI);

	float SpreadY = FMath::Cos(RotationAngle);
	float SpreadZ = FMath::Sin(RotationAngle);

	FVector Result = (ShotRotation.RotateVector(FVector::UpVector) * SpreadZ
					+ ShotRotation.RotateVector(FVector::RightVector) * SpreadY) * SpreadSize;
	return Result;
}


APawn* UWeaponBarellComponent::GetPawnOwner() const
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	if(!IsValid(PawnOwner))
	{
		PawnOwner = Cast<APawn>(GetOwner()->GetOwner());
	}
	return PawnOwner;
}

AController* UWeaponBarellComponent::GetController() const
{
	APawn* PawnOwner = GetPawnOwner();
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}
