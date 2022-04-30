// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/TimelineComponent.h"


#include "BasePlatform.generated.h"

UENUM(BlueprintType)
enum class EPlatformBehavior : uint8
{
	OnDemand, // Platform movement only when query
	Loop // Platform movement by loop
};

UENUM(BlueprintType)
enum class EPlatformDirection : uint8
{
	Stop,
	Play,
	Reverse
};

UCLASS()
class HOMEWORK_API ABasePlatform : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABasePlatform();

protected:

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly , Category = "Platform")
	class APlatformTrigger* PlatformTrigger;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Platform")
	UStaticMeshComponent* PlatformMesh;

	// Point in which the end of movement
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, meta = (MakeEditWidget))
	FVector EndLocation;

	// Point in which the start of movement
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FVector StartLocation;

	// Selection behavior a platform
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	EPlatformBehavior PlatformBehavior = EPlatformBehavior::OnDemand;

	// Timeline for relocation a platform
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Timeline")
	UCurveFloat* PlatformMovementCurve;

	// Delay platform before movement
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 0.1))
	float CooldownPlatform = 0.1f;

	// Speed platform after begin movement
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 0.1))
	float StartSpeedPlatform = 0.1f;

	// Speed platform after begin reverse movement
	UPROPERTY(EditAnywhere, Category = "Settings", meta = (ClampMin = 0.1))
	float ReverseSpeedPlatform = 0.1f;

	// Is it worth using timer
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bIsUseTimer = false;

	UPROPERTY(BlueprintReadWrite)
	bool bIsPlatformForward = true;

	UFUNCTION(BlueprintCallable)
	void MovePlatformBPCallable();

	UFUNCTION()
	void MovePlatformEvent(bool bIsActivated_In);
 
private:
	
/* 	necessary using for update location
	a platform every call the function */
	UFUNCTION()
	void UpdateTimelinePlatform(float Value);

/*	conditions update location platform  */
	UFUNCTION()
	void OnTimelineFinished();

	FTimerHandle PlatformReverseTimerHandle;

	bool bCanMove = true;

/** Three type the function 
	for update location a platform **/
	void MovementPlatformStart();

	void MovementPlatformPlay();

	void MovementPlatformReverse();
	
	// Timeline needful for visible movement a platform
	UPROPERTY( )
	FTimeline PlatformMovementTimeline;

	FORCEINLINE void StartPlatform() { PlatformMovementTimeline.PlayFromStart(); }

	// interface for call function "Play" in order to, 
	// it could be movement platform of start location in end location
	FORCEINLINE void PlayMovmentPlatform() { PlatformMovementTimeline.Play(); };

	// interface for call function "Reverse" in order to,
	// it could be movement platform of end location in start location
	FORCEINLINE void ReverseMovementPlatform() { PlatformMovementTimeline.Reverse(); };	

	void CooldownPlatformElapsed();

	void MovePlatform();

	bool bIsLoopMovementPlatform;

	
};
