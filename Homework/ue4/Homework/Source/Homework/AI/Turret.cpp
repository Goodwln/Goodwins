// Fill out your copyright notice in the Description page of Project Settings.


#include "Turret.h"

#include "AIController.h"
#include "Components/AIComponents/ItemAttributeComponent.h"
#include "Components/Weapon/WeaponBarellComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"
#include "Perception/AISense_Damage.h"

// Sets default values
ATurret::ATurret()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	USceneComponent* TurretRoot = CreateDefaultSubobject<USceneComponent>(TEXT("TurretRoot"));
	SetRootComponent(TurretRoot);

	TurretBaseComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBase"));
	TurretBaseComponent->SetupAttachment(TurretRoot);
	
	TurretBarrelComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TurretBarrel"));
	TurretBarrelComponent->SetupAttachment(TurretBaseComponent);

	WeaponBarrel = CreateDefaultSubobject<UWeaponBarellComponent>(TEXT("WeaponBarrel"));
	WeaponBarrel->SetupAttachment(TurretBarrelComponent);

	AttributeComponent = CreateDefaultSubobject<UItemAttributeComponent>(TEXT("AttributeComponent"));
 
}

// Called every frame
void ATurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
		switch (CurrentTurretState)
		{
		case ETurretState::Searching:
			{
				SearchingMovement(DeltaTime);
				break;	
			}
		case ETurretState::Tracking:
			{
				TrackingMovement(DeltaTime);
				break;
			}
		case ETurretState::None:
			{
				break;
			}
		}
}

void ATurret::BeginPlay()
{
	Super::BeginPlay();
	
	SetCurrentTurretState(ETurretState::Searching);

	AttributeComponent->OnItemDeathEvent.AddDynamic(this, &ATurret::OnDeath);
	AttributeComponent->OnItemTakeDamageEvent.AddDynamic(this, &ATurret::TakeAnyDamage);
}

void ATurret::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AAIController* AIController = Cast<AAIController>(NewController);
	if(IsValid(AIController))
	{
		FGenericTeamId TeamId((uint8)Team);
		AIController->SetGenericTeamId(TeamId);
	}
}

void ATurret::OnCurrentTargetSet()
{
	const ETurretState NewTurretState = IsValid(CurrentTarget) ? ETurretState::Tracking : ETurretState::Searching;
	SetCurrentTurretState(NewTurretState);
}

FVector ATurret::GetPawnViewLocation() const
{
	return WeaponBarrel->GetComponentLocation();
}

FRotator ATurret::GetViewRotation() const
{
	return WeaponBarrel->GetComponentRotation();
}

bool ATurret::IsDestroy() const
{
	return AttributeComponent->IsDeath();
}

void ATurret::MakeShot()
{
	
	FVector ShotLocation = WeaponBarrel->GetComponentLocation();
	FVector ShotDirection = WeaponBarrel->GetComponentRotation().RotateVector(FVector::ForwardVector);
	float SpreadAngle = FMath::DegreesToRadians(BulletSpreadAngle);
	
	WeaponBarrel->ChangeWeaponBarrelProperty(EWeaponMode::FirstMode);
	WeaponBarrel->Shot(ShotLocation,ShotDirection, SpreadAngle);
}

void ATurret::OnDeath()
{
	SetCurrentTurretState(ETurretState::None);
	
	if(IsValid(DestroyParticle))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), DestroyParticle, GetActorLocation());
	}
	
}
 

void ATurret::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy,
                            AActor* DamageCauser)
{
	UAISense_Damage::ReportDamageEvent(
		GetWorld(),
		DamagedActor,
		DamageCauser,
		Damage,
		DamageCauser->GetActorLocation(),
		DamagedActor->GetActorLocation());
}

 

void ATurret::SearchingMovement(float DeltaTime)
{
	FRotator TurretBaseRotation = TurretBaseComponent->GetRelativeRotation();
	TurretBaseRotation.Yaw += DeltaTime * BaseSearchingRotationRate;
	TurretBaseComponent->SetRelativeRotation(TurretBaseRotation);

	FRotator TurretBarrelRotation = TurretBarrelComponent->GetRelativeRotation();
	TurretBarrelRotation.Pitch = FMath::FInterpTo(TurretBarrelRotation.Pitch, 0.f, DeltaTime, BarrelPitchRotationRate);
	TurretBarrelComponent->SetRelativeRotation(TurretBarrelRotation);
}

void ATurret::TrackingMovement(float DeltaTime)
{
	FVector BaseLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBaseComponent->GetComponentLocation()).GetSafeNormal2D();
	FQuat LookAtQuat = BaseLookAtDirection.ToOrientationQuat();
	FQuat TargetQuat = FMath::QInterpTo(TurretBaseComponent->GetComponentQuat(), LookAtQuat, DeltaTime, BaseTrackingInterpSpeed);
	TurretBaseComponent->SetWorldRotation(TargetQuat);

	FVector BarrelLookAtDirection = (CurrentTarget->GetActorLocation() - TurretBarrelComponent->GetComponentLocation()).GetSafeNormal();
	float LookAtPitchAngle = BarrelLookAtDirection.ToOrientationRotator().Pitch;

	FRotator BarrelRelativeRotation = TurretBarrelComponent->GetRelativeRotation();
	BarrelRelativeRotation.Pitch = FMath::FInterpTo(BarrelRelativeRotation.Pitch, LookAtPitchAngle, DeltaTime, BarrelPitchRotationRate);

	TurretBarrelComponent->SetRelativeRotation(BarrelRelativeRotation);
}

void ATurret::SetCurrentTurretState(ETurretState NewState)
{
	bool bIsStateChanged = NewState != CurrentTurretState;


	CurrentTurretState = NewState;

	if(!bIsStateChanged)
	{
		return;
	}

	switch (CurrentTurretState)
	{
	case ETurretState::Searching:
		{
			GetWorldTimerManager().ClearTimer(ShotTimer);
			break;	
		}
	case ETurretState::Tracking:
		{
			GetWorldTimerManager().SetTimer(ShotTimer, this, &ATurret::MakeShot, GetFireInterval(), true, FireDelayTime);
			break;
		}
	case ETurretState::None:
		{
			GetWorldTimerManager().ClearTimer(ShotTimer);
			break;
		}
	}
}

float ATurret::GetFireInterval() const
{
	return 60 / RateOfFire;
}
 

