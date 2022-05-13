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
#include "Characters/BaseCharacter.h"
#include "Net/UnrealNetwork.h"

DEFINE_LOG_CATEGORY_STATIC(LogWeaponBarellComponent, All, All);

UWeaponBarellComponent::UWeaponBarellComponent()
{
	SetIsReplicatedByDefault(true);
}

void UWeaponBarellComponent::TraceFXPainting(FVector ShotEnd)
{
	FVector MuzzleStart = GetComponentLocation();
	 
	if(IsValid(CurrentWeaponBarrelProperty.MuzzleFlashFX))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
							     GetWorld(),
							     CurrentWeaponBarrelProperty.MuzzleFlashFX,
							     MuzzleStart,
							     GetComponentRotation());
	}

	if(IsValid(CurrentWeaponBarrelProperty.TraceFX))
	{
		UNiagaraComponent* TraceFXComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
																  GetWorld(),
																  CurrentWeaponBarrelProperty.TraceFX,
																  MuzzleStart,
																  GetComponentRotation());
		TraceFXComponent->SetVectorParameter(FXParamTraceEnd, ShotEnd);
	}
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

void UWeaponBarellComponent::InitProjectileForCurrentWeapon()
{
	check(AllWeaponBarrelProperty.Num() > 0);
	
	for (auto &WeaponBarrelPropertyElem : AllWeaponBarrelProperty)
	{
		if(!IsValid(WeaponBarrelPropertyElem.Value.ProjectileClass))
		{
			continue;
		}
		const int32 PoolSize = WeaponBarrelPropertyElem.Value.ProjectilePoolSize;
		

		for(int i = 0; i < PoolSize; ++i)
		{
			ABaseProjectile* Projectile = GetWorld()->SpawnActor<ABaseProjectile>(WeaponBarrelPropertyElem.Value.ProjectileClass, ProjectilePoolLocation, FRotator::ZeroRotator);
			Projectile->SetOwner(GetPawnOwner());
			Projectile->SetProjectileActive(true);
			Projectile->SetActorEnableCollision(false);
			WeaponBarrelPropertyElem.Value.ProjectilePool.Add(Projectile);
 		}
	}
}


void UWeaponBarellComponent::BeginPlay()
{
	Super::BeginPlay();
	InitProjectileForCurrentWeapon();
}

void UWeaponBarellComponent::Server_Shot_Implementation(FVector ShotStart, FVector ShotDirection, float SpreadAngle)
{
 	if(GetPawnOwner()->GetLocalRole() == ROLE_Authority)
	{
		NetMulticast_Shot(ShotStart, ShotDirection,SpreadAngle);

	}
}


void UWeaponBarellComponent::NetMulticast_Shot_Implementation(FVector ShotStart, FVector ShotDirection, float SpreadAngle)
{
	ShotReplicated(ShotStart, ShotDirection,SpreadAngle);
}

void UWeaponBarellComponent::ShotReplicated(FVector ShotStart, FVector ShotDirection, float SpreadAngle)
{
	for (int i = 0; i < CurrentWeaponBarrelProperty.BulletPerShot; i++)
	{
		switch (CurrentWeaponBarrelProperty.HitRegistration)
		{
		case EHitRegistrationType::HitScan:
			{
				HitScan(ShotStart, ShotDirection, SpreadAngle);
				break;
			}
		case EHitRegistrationType::Projectile:
			{
				ProjectileScan(ShotStart, ShotDirection);
				break;
			}
		case EHitRegistrationType::None:
			{
				break;
			}
		}
	}
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

void UWeaponBarellComponent::LaunchProjectile(const FVector& LaunchStart, const FVector& Direction)
{
	if(++CurrentProjectileIndex >= CurrentWeaponBarrelProperty.ProjectilePoolSize)
	{
		CurrentProjectileIndex = 0;
	}
	
	ABaseProjectile* Projectile = CurrentWeaponBarrelProperty.ProjectilePool[CurrentProjectileIndex];
	Projectile->SetActorLocation(LaunchStart);
	Projectile->SetProjectileActive(true);
	Projectile->LaunchProjectile(Direction.GetSafeNormal());
	Projectile->SetActorEnableCollision(true);
	Projectile->OnProjectileHit.AddDynamic(this, &UWeaponBarellComponent::ProcessProjectileHit);
}

void UWeaponBarellComponent::ProcessHit(const FHitResult& HitResult, const FVector& Direction)
{
	FVector HitLocation = HitResult.ImpactPoint;
	AActor* HitActor = Cast<AActor>( HitResult.GetActor());
	if (GetOwner()->HasAuthority() && IsValid(HitActor))
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

	ShotEnd = TraceResult.ImpactPoint;
	ShotStart = GetComponentLocation();
	
	if(TraceResult.ImpactPoint == FVector::ZeroVector)
	{
		float Offset = 2000.f;
		ShotEnd = ShotStart + ShotDirection * Offset;
	}
	FVector Direction = ShotEnd - ShotStart;
	LaunchProjectile(ShotStart, Direction);
}

void UWeaponBarellComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UWeaponBarellComponent, CurrentWeaponBarrelProperty);
	DOREPLIFETIME(UWeaponBarellComponent, CurrentWeaponMode);
 }

void UWeaponBarellComponent::Shot(FVector& ShotStart, FVector& ShotDirection, float SpreadAngle)
{
	//Server_Shot(ShotStart, ShotDirection, SpreadAngle);
	ShotReplicated(ShotStart, ShotDirection,SpreadAngle);
}


float UWeaponBarellComponent::GetBulletPerShot() const
{
	return CurrentWeaponBarrelProperty.BulletPerShot;
}

EHitRegistrationType UWeaponBarellComponent::GetHitRegistration() const
{
	return CurrentWeaponBarrelProperty.HitRegistration;
}

void UWeaponBarellComponent::ChangeWeaponBarrelProperty(EWeaponMode CurrentWeaponMode_In)
{
	CurrentWeaponMode = CurrentWeaponMode_In;
	for(const auto &Elem : AllWeaponBarrelProperty)
	{
		if(Elem.Key == CurrentWeaponMode_In)
		{
			CurrentWeaponBarrelProperty = Elem.Value;
		}
	}
}

void UWeaponBarellComponent::Server_ChangeWeaponBarrelProperty_Implementation(EWeaponMode CurrentWeaponMode_In)
{
	ChangeWeaponBarrelProperty(CurrentWeaponMode_In);
}


void UWeaponBarellComponent::ProcessProjectileHit(ABaseProjectile* Projectile, const FHitResult& HitResult,
                                                  const FVector& Direction)
{
	ProcessHit(HitResult, Direction);
	Projectile->ProjectileDisable(ProjectilePoolLocation,FRotator::ZeroRotator, Projectile->GetTimerDetonation() + 0.3f);
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
