// Fill out your copyright notice in the Description page of Project Settings.


#include "WallDetectorComponent.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "Homework/HomeworkTypes.h"
#include "Homework/Subsystems/DebugSubsystem.h"
#include "Homework/Utils/TraceUtils.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Homework/Characters/BaseCharacter.h"
#include "Homework/Characters/BaseCharacterMovementComponent.h"

// Called when the game starts
void UWallDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("UWallDetectorComponent::BeginPlay() only ABaseCharacter can use UWallDetectorComponent"));
	CacheCharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());
	CacheCharacterOwner->GetCapsuleComponent()->OnComponentHit.AddDynamic(this, &UWallDetectorComponent::CapsuleOverlap);
}
 
void UWallDetectorComponent::CheckSide(const FVector HitNormal)
{
	const float Angle = FMath::RadiansToDegrees(FMath::Acos(HitNormal.Z));
	const float DotSide = FVector::DotProduct(CacheCharacterOwner->GetActorRightVector(), HitNormal);
	if(DotSide > 0.f)
	{
		WallDescription.WallSide = ESide::Left;
		WallDescription.AngleRotation = Angle;
	}
	else
	{
		WallDescription.WallSide = ESide::Right;
		WallDescription.AngleRotation = Angle;
	}
	WallDescription.CurrentWallSide = DotSide;
	WallDescription.bIsWall = true;
}

bool UWallDetectorComponent::IsWall(const FVector HitNormal)
{
	if(HitNormal.Z > CacheCharacterOwner->GetBaseCharacterMovementComponent()->GetWalkableFloorZ() || HitNormal.Z < -0.0005)
	{
		return true;
	}
	return false;
}

void UWallDetectorComponent::GetWallDescription(FWallDescription& OutWallDescription)
{
	WallDescription.bIsWall = DetectWall();
	OutWallDescription = WallDescription;
}

bool UWallDetectorComponent::DetectWall()
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());

#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubSystem->IsCategoryEnable(DebugCategoryWallDetection);
#else
	bool bIsDebugEnabled = false;
#endif
	float DrawTime = 2.f;
	
	float WallOffset = 100.f;
	if(WallDescription.WallSide == ESide::Right)
	{
		FHitResult Hit;
		const FVector Start = CacheCharacterOwner->GetActorLocation();
		const FVector End = CacheCharacterOwner->GetActorLocation() + CacheCharacterOwner->GetActorUpVector() * -CacheCharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + CacheCharacterOwner->GetActorRightVector() * WallOffset;
		if(TraceUtils::LineTraceSingleByChanel(GetWorld() ,Hit ,Start ,End ,ECC_WallRunnable ,QueryParams ,FCollisionResponseParams::DefaultResponseParam , bIsDebugEnabled , DrawTime , FColor::Green))
		{
			return true;
		}
	}

	if(WallDescription.WallSide == ESide::Left)
	{
		FHitResult Hit;
		const FVector Start = CacheCharacterOwner->GetActorLocation();
		const FVector End = CacheCharacterOwner->GetActorLocation() + CacheCharacterOwner->GetActorUpVector() * -CacheCharacterOwner->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight() + CacheCharacterOwner->GetActorRightVector() * -WallOffset;
		if(TraceUtils::LineTraceSingleByChanel(GetWorld() ,Hit ,Start ,End ,ECC_WallRunnable ,QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DrawTime, FColor::Green))
		{
			return true;
		}
	}
	return  false;
}

void UWallDetectorComponent::CapsuleOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor,
                                            UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	const FVector HitNormal = Hit.ImpactNormal;
	if (IsWall(HitNormal))
	{
		WallDescription.WallSide = ESide::None;
		return;
	}
	CheckSide(HitNormal);
}




