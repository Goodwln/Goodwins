// Fill out your copyright notice in the Description page of Project Settings.


#include "FloorDetectorComponent.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Homework/HomeworkTypes.h"
#include "Homework/Characters/BaseCharacter.h"
#include "Homework/Characters/BaseCharacterMovementComponent.h"
#include "Homework/Subsystems/DebugSubsystem.h"
#include "Homework/Utils/TraceUtils.h"
#include "Kismet/GameplayStatics.h"

// Called when the game starts
void UFloorDetectorComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("UFloorDetectorComponent::BeginPlay() can use when GetOwner == ABaseCharacter"));
	CacheBaseCharacter = StaticCast<ABaseCharacter*>(GetOwner());
}

bool UFloorDetectorComponent::DetectFloor(FFloorProperty& OutFloorProperty)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());
	
#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnable = DebugSubsystem->IsCategoryEnable(DebugCategoryFloorDetection);
#else
	bool bIsDebugEnable = false;
#endif
	float DrawTime = 2.f;
	
	UCapsuleComponent* CapsuleComponent = CacheBaseCharacter->GetCapsuleComponent();
	FVector Start = CapsuleComponent->GetComponentLocation();
	FVector End = CapsuleComponent->GetComponentLocation() + CacheBaseCharacter->GetActorUpVector() * -CapsuleComponent->GetScaledCapsuleHalfHeight();
	FQuat Rotation = FQuat(0.f, 0.f, 0.f,1.f);
	float Radius = CapsuleComponent->GetScaledCapsuleRadius();
	float HalfHeight = CapsuleComponent->GetScaledCapsuleHalfHeight();
	
	FHitResult Hit;
	if(!TraceUtils::SweepCapsuleSingleByChanel(GetWorld(),Hit,Start, End, Radius,HalfHeight, Rotation, ECC_FloorSlide, QueryParams, FCollisionResponseParams::DefaultResponseParam, bIsDebugEnable, DrawTime))
	{
		return false;
	}

	const float Dot = FVector::DotProduct(FVector::UpVector, Hit.ImpactNormal);
	const float Angle = FMath::RadiansToDegrees(FMath::Acos(Dot));
	if(Angle > 70.f)
	{
		OutFloorProperty.Angle = 0.f;
		return false;
	}
	
	OutFloorProperty.Angle = Angle;
	return true;
}


bool UFloorDetectorComponent::DetectСeiling()
{

	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());
	
#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnable = DebugSubsystem->IsCategoryEnable(DebugCategoryCeilingDetection);
#else
	bool bIsDebugEnable = false;
#endif
	float DrawTime = 2.f;

	ACharacter* DefaultCharacter = CacheBaseCharacter->GetClass()->GetDefaultObject<ACharacter>();
	UCapsuleComponent* CapsuleComponent = CacheBaseCharacter->GetCapsuleComponent();
	
	FVector Position = CapsuleComponent->GetComponentLocation();
	Position.Z += 30.f;
	float HalfHeight = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleHalfHeight();
	float Radius = DefaultCharacter->GetCapsuleComponent()->GetUnscaledCapsuleRadius();
	if(TraceUtils::OverlapBlockingCapsuleTestByProfile(GetWorld(),Position,Radius,HalfHeight,FQuat::Identity,CollisionProfilePawn, QueryParams, bIsDebugEnable, DrawTime))
	{
		return true;
	}
	return  false;
}

bool UFloorDetectorComponent::DetectForwardBarrier()
{
	FCollisionQueryParams QueryParams;
	QueryParams.bTraceComplex = true;
	QueryParams.AddIgnoredActor(GetOwner());
	
#if ENABLE_DRAW_DEBUG
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	bool bIsDebugEnable = DebugSubsystem->IsCategoryEnable(DebugCategoryCeilingDetection);
#else
	bool bIsDebugEnable = false;
#endif
	float DrawTime = 2.f;

	FVector Start = CacheBaseCharacter->GetActorLocation();
	Start.Z -= CacheBaseCharacter->GetBaseCharacterMovementComponent()->GetCurrentCapsuleSize(ECurrentCapsuleSize::SlideCapsule).HalfHeight * 0.3f;

	float Offset = 70.f;
	FVector End = Start + CacheBaseCharacter->GetActorForwardVector() * Offset;
	FHitResult Hit;
	if(!TraceUtils::LineTraceSingleByChanel(GetWorld(),Hit, Start, End, ECC_FloorSlide, QueryParams,FCollisionResponseParams::DefaultResponseParam, bIsDebugEnable, DrawTime, FColor::Red))
	{
		return false;
	}

	return true;
}
