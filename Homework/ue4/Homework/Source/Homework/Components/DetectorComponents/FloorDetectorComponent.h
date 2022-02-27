// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Homework/Characters/BaseCharacter.h"
#include "FloorDetectorComponent.generated.h"



UENUM(BlueprintType)
enum class ECurrentSlope : uint8
{
	None = 0,
	Forward,
	Back
};

USTRUCT()
struct FFloorProperty
{
	GENERATED_BODY()
	
	float Angle = 0.f;
	ECurrentSlope CurrentSlope = ECurrentSlope::None;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK_API UFloorDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	bool DetectFloor(FFloorProperty& OutFloorProperty);
	
	bool DetectСeiling();

	bool DetectForwardBarrier();
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY()
	class ABaseCharacter* CacheBaseCharacter;
	FFloorProperty FloorProperty;
};


