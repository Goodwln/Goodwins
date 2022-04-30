// Fill out your copyright notice in the Description page of Project Settings.


#include "MeleeHitRegistrator.h"

#include "HomeworkTypes.h"
#include "Kismet/GameplayStatics.h"
#include "Subsystems/DebugSubsystem.h"
#include "Utils/TraceUtils.h"

UMeleeHitRegistrator::UMeleeHitRegistrator()
{
	PrimaryComponentTick.bCanEverTick = true;
	SphereRadius = 5.f;
	SetCollisionProfileName(CollisionProfileNoCollision);
	
}

void UMeleeHitRegistrator::TickComponent(float DeltaTime, ELevelTick Tick,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, Tick, ThisTickFunction);
	if(bIsHitRegistrationEnabled)
	{
		ProcessHitRegistration();
	}
	PreviousComponentLocation = GetComponentLocation();
}

void UMeleeHitRegistrator::ProcessHitRegistration()
{
	FVector CurrentLocation = GetComponentLocation();

	FCollisionQueryParams CollisionQueryParams;
	CollisionQueryParams.bTraceComplex = true;
	CollisionQueryParams.AddIgnoredActor(GetOwner()->GetOwner());
	CollisionQueryParams.AddIgnoredActor(GetOwner());

#if UE_BUILD_DEVELOPMENT || UE_BUILD_DEBUG
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubsystem->IsCategoryEnable(DebugCategoryMeleeHitDetection);
#else
	bool bIsDebugEnabled = false;
#endif
	float DrawTime = 5.f;

	
	FHitResult HitResult;

	bool bHasHit = TraceUtils::SweepSphereSingleByChanel(
			GetWorld(),
			HitResult,
			PreviousComponentLocation,
			CurrentLocation,
			GetScaledSphereRadius(),
			ECC_Melee,
			CollisionQueryParams,
			FCollisionResponseParams::DefaultResponseParam,
			bIsDebugEnabled,
			DrawTime
	);

	if(bHasHit)
	{
		FVector Direction = (CurrentLocation - PreviousComponentLocation).GetSafeNormal();
		if(OnMeleeHitRegistred.IsBound())
		{
			OnMeleeHitRegistred.Broadcast(HitResult, Direction);
		}
	}
}

void UMeleeHitRegistrator::SetIsHitRegistrationEnable(bool bIsEnabled_In)
{
	bIsHitRegistrationEnabled = bIsEnabled_In;
}
