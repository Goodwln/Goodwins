// Fill out your copyright notice in the Description page of Project Settings.


#include "LedgeDetectorComponent.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "../../HomeworkTypes.h"
#include "DrawDebugHelpers.h"
#include "../../Utils/TraceUtils.h"
#include "../../BaseGameInstance.h"
#include "Kismet/GameplayStatics.h"
#include "../../Subsystems/DebugSubsystem.h"

void ULedgeDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetOwner()->IsA<ACharacter>(), TEXT("ULedgeDetectorComponent::BeginPlay() only ACharacter can use ULedgeDetectorComponent"));
	CachedCharacterOwner = StaticCast<ACharacter*>(GetOwner());
}

bool ULedgeDetectorComponent::DetectLedge(OUT FLedgeDescription& LedgeDescription)
{
	UCapsuleComponent* CapsuleComponent = CachedCharacterOwner->GetCapsuleComponent();
	
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());
                          
#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubSystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnabled = DebugSubSystem->IsCategoryEnable(DebugCategoryLedgeDetection);
#else
	bool bIsDebugEnabled = false;
#endif
	float DrawTime = 2.f;

	float BottomZOffset = 2.0f;
	FVector CharacterBottom = CachedCharacterOwner->GetActorLocation() - (CapsuleComponent->GetScaledCapsuleHalfHeight() - BottomZOffset) * FVector::UpVector;

	//1. Forward Check
	FHitResult ForwardCheckHitResult;
	if( !ForwardCheck(CapsuleComponent, QueryParams, bIsDebugEnabled, DrawTime, CharacterBottom, ForwardCheckHitResult))
	{
		return false;
	}

	//2. Downward Check
	FHitResult DownwardCheckHitResult;
	if( !DownwardCheck(CapsuleComponent, QueryParams, bIsDebugEnabled, DrawTime, CharacterBottom, ForwardCheckHitResult,DownwardCheckHitResult))
	{
		return false;
	}
	
	// 3. Overlap Check
	FVector OverlapLocation;
	if (!OverlapCheck(CapsuleComponent, QueryParams, bIsDebugEnabled, DrawTime, DownwardCheckHitResult, OverlapLocation))
	{
		return false;
	}

	UPrimitiveComponent* OwnerLedge = DownwardCheckHitResult.GetComponent();
	LedgeDescription.ActorLedge = OwnerLedge;
	
	float MantlingHeightOffset = 15.f;
	LedgeDescription.MantlingHeight = (CharacterBottom + DownwardCheckHitResult.ImpactPoint).Z - MantlingHeightOffset;
	
	LedgeDescription.Location = OverlapLocation;
	LedgeDescription.Rotation = (ForwardCheckHitResult.ImpactNormal * FVector(-1.f, -1.f, 0.f)).ToOrientationRotator();
	LedgeDescription.LedgeNormal = ForwardCheckHitResult.ImpactNormal;

	return true;
}

bool ULedgeDetectorComponent::ForwardCheck(UCapsuleComponent* CapsuleComponent, FCollisionQueryParams QuaryParams, bool bIsDebugEnabled, float DrawTime, FVector CharacterBottom, FHitResult& ForwardCheckHitResult)
{
	// 1. Forward Check
	const float ForwardCheckCapsuleRadius = CapsuleComponent->GetScaledCapsuleRadius();
	const float ForwardCheckCapsuleHalfHeight = (MaximumLedgeHeight - MinimumLedgeHeight) * 0.5f;

	const FVector ForwardStartLocation = CharacterBottom + (MinimumLedgeHeight + ForwardCheckCapsuleHalfHeight) * FVector::UpVector;

	const FVector ForwardEndLocation = ForwardStartLocation + CachedCharacterOwner->GetActorForwardVector() * ForwardCheckDistance;

	if (!TraceUtils::SweepCapsuleSingleByChanel(GetWorld(), ForwardCheckHitResult, ForwardStartLocation, ForwardEndLocation, ForwardCheckCapsuleRadius, ForwardCheckCapsuleHalfHeight, FQuat::Identity, ECC_Climbing, QuaryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DrawTime, FColor::Green) )
	{
		return false;
	}
	return true;
}

bool ULedgeDetectorComponent::DownwardCheck(UCapsuleComponent* CapsuleComponent, FCollisionQueryParams QuaryParams, bool bIsDebugEnabled, float DrawTime, FVector CharacterBottom, FHitResult ForwardCheckHitResult, FHitResult& DownwardCheckHitResult)
{
	//2. Downward Check
	float DownwardSphereCheckRadius = CapsuleComponent->GetScaledCapsuleRadius();
	
	float DownwardCheckDepthOffset = 10.f;
	FVector DownwardStartLocation = ForwardCheckHitResult.ImpactPoint - ForwardCheckHitResult.ImpactNormal * DownwardCheckDepthOffset;
	DownwardStartLocation.Z = CharacterBottom.Z + MaximumLedgeHeight + DownwardSphereCheckRadius;
	
	FVector DownwardEndLocation(DownwardStartLocation.X, DownwardStartLocation.Y, CharacterBottom.Z);
 	if (!TraceUtils::SweepSphereSingleByChanel(GetWorld(), DownwardCheckHitResult, DownwardStartLocation, DownwardEndLocation, DownwardSphereCheckRadius, ECC_Climbing, QuaryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnabled, DrawTime))
	{
		return false;
	}
	return true;
}

bool ULedgeDetectorComponent::OverlapCheck(UCapsuleComponent* CapsuleComponent, FCollisionQueryParams QuaryParams, bool bIsDebugEnabled, float DrawTime, FHitResult DownwardCheckHitResult, FVector& OverlapLocation)
{
	// 3. Overlap Check
	ACharacter* DefaultCharacter = CachedCharacterOwner->GetClass()->GetDefaultObject<ACharacter>();
	UCapsuleComponent* DefaultCapsuleComponent = DefaultCharacter->GetCapsuleComponent();
	
	float OverlapCapsuleRadius = DefaultCapsuleComponent->GetUnscaledCapsuleRadius();
	float OverlapCapsuleHalfHeight = DefaultCapsuleComponent->GetUnscaledCapsuleHalfHeight();

	float OverlapCapsuleFloorOffset = 2.f;
	OverlapLocation = DownwardCheckHitResult.ImpactPoint + (OverlapCapsuleHalfHeight + OverlapCapsuleFloorOffset) * FVector::UpVector;

	if (TraceUtils::OverlapBlockingCapsuleTestByProfile(GetWorld(), OverlapLocation, OverlapCapsuleRadius, OverlapCapsuleHalfHeight, FQuat::Identity, CollisionProfilePawn, QuaryParams, bIsDebugEnabled, DrawTime))
	{
		return false;
	}
	return true;
}