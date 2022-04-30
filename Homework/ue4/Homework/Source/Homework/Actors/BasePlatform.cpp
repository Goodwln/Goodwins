// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlatform.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/TimelineComponent.h"
#include "Environment/PlatformTrigger.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABasePlatform::ABasePlatform()
{
 
	PrimaryActorTick.bCanEverTick = true;
 
	USceneComponent* DefaultPlatformRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Platform root"));
	RootComponent = DefaultPlatformRoot;

	PlatformMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Platform"));
	PlatformMesh->SetupAttachment(RootComponent);
}

void ABasePlatform::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(PlatformTrigger))
	{
		PlatformTrigger->OnTriggerActivated.AddDynamic(this, &ABasePlatform::MovePlatformEvent);
		UE_LOG(LogTemp, Warning, TEXT("kek"));
	}
	
	// Validation curve for current functioning
	if (IsValid(PlatformMovementCurve))
	{
		// Timeline float delegate for working platform movement
		FOnTimelineFloat OnTimelineCallBack;
		
		// Give the function for timeline float delegete for working platform movement
		OnTimelineCallBack.BindUFunction(this, FName("UpdateTimelinePlatform"));

		// For working platform movement necessary give the timeline type float and curve
		PlatformMovementTimeline.AddInterpFloat(PlatformMovementCurve, OnTimelineCallBack, FName("Alpha"));

		// Timeline Event delegate necessary for looping platform movement
		FOnTimelineEvent OnTimelineFinish;

		// Give the function for timeline event delegate for working looping platform movement
		OnTimelineFinish.BindUFunction(this, FName("OnTimelineFinished"));
		
		// Main timeline give the timeline event for return in start location of end location through reverse 
		PlatformMovementTimeline.SetTimelineFinishedFunc(OnTimelineFinish);
	
		// Set start location get relative location
		StartLocation = PlatformMesh->GetRelativeLocation();
		
		// Need for begin platform movement
		if (PlatformBehavior == EPlatformBehavior::Loop)
		{
			MovementPlatformStart();
		}
	}
}


void ABasePlatform::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
	PlatformMovementTimeline.TickTimeline(DeltaSeconds);
}


void ABasePlatform::MovePlatformBPCallable()
{
	if (bIsPlatformForward)
	{
		MovementPlatformPlay();
	}
	else
	{
		MovementPlatformReverse();
	}
}

void ABasePlatform::MovePlatformEvent(bool bIsActivated_In)
{
	if (bIsActivated_In)
	{
		MovementPlatformPlay();
	}
	else
	{
		MovementPlatformReverse();
	}
}
 

void ABasePlatform::UpdateTimelinePlatform(float Value)
{

		PlatformMesh->SetRelativeLocation(FMath::Lerp(StartLocation, EndLocation, Value));
 
	
}


void ABasePlatform::OnTimelineFinished()
{	
	bIsPlatformForward = !bIsPlatformForward;

	if (bIsUseTimer)
	{
		bCanMove = false;

		GetWorld()->GetTimerManager().SetTimer(PlatformReverseTimerHandle, this, &ABasePlatform::CooldownPlatformElapsed, CooldownPlatform, false);

		return;
	}

	if (PlatformBehavior == EPlatformBehavior::Loop)
	{
		MovePlatform();
	}	
}

void ABasePlatform::CooldownPlatformElapsed()
{
	bCanMove = true;

	if (PlatformBehavior == EPlatformBehavior::Loop)
	{
		MovePlatform();
	}
}

void ABasePlatform::MovePlatform()
{
	if (bIsPlatformForward && FMath::IsNearlyEqual(PlatformMovementTimeline.GetPlaybackPosition(), 0.0f))
	{
		MovementPlatformPlay();
	}
	else
	{
		MovementPlatformReverse();
	}
}



void ABasePlatform::MovementPlatformStart()
{
	if (!bCanMove)
	{
		return;
	}

	PlatformMovementTimeline.SetPlayRate(StartSpeedPlatform);

	PlatformMovementTimeline.PlayFromStart();
}

void ABasePlatform::MovementPlatformPlay()
{
	if (!bCanMove)
	{
		return;
	}

	PlatformMovementTimeline.SetPlayRate(StartSpeedPlatform);

	PlayMovmentPlatform();
}



void ABasePlatform::MovementPlatformReverse()
{
	if (!bCanMove)
	{
		return;
	}

	PlatformMovementTimeline.SetPlayRate(ReverseSpeedPlatform);

	ReverseMovementPlatform();
}




