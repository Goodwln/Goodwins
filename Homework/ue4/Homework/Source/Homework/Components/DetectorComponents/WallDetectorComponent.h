// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "WallDetectorComponent.generated.h"




UENUM()
enum class ESide : uint8
{
	None = 0 UMETA(DisplayName = "None"),
	Left UMETA(DisplayName = "Left"), 
	Right UMETA(DisplayName = "Right"),
};

USTRUCT(BlueprintType)
struct FWallDescription
{
	GENERATED_BODY()

	bool bIsWall = false;
	
	float AngleRotation = 0.f;
	
	ESide WallSide = ESide::None;

	float CurrentWallSide = 0.f;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK_API UWallDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void CheckSide(FVector HitNormal);
	bool IsWall(FVector HitNormal);
	
public:	
	TWeakObjectPtr<class ABaseCharacter> CacheCharacterOwner;

	void GetWallDescription(OUT FWallDescription& OutWallDescription);

	bool DetectWall();
	
private:
	UFUNCTION()
	void CapsuleOverlap(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	FWallDescription WallDescription;
};




