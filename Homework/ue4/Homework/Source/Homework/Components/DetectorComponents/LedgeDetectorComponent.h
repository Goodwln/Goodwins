// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Components/CapsuleComponent.h"
#include "LedgeDetectorComponent.generated.h"

USTRUCT(BlueprintType)
struct FLedgeDescription
{
	GENERATED_BODY()
public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LedgeDescription")
	FVector Location;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LedgeDescription")
	FVector LedgeNormal;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LedgeDescription")
	FRotator Rotation;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LedgeDescription")
	float MantlingHeight = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "LedgeDescription")
	class UPrimitiveComponent* ActorLedge;
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK_API ULedgeDetectorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	bool DetectLedge(OUT FLedgeDescription& LedgeDescription);

protected:
	virtual void BeginPlay() override;
	bool ForwardCheck(UCapsuleComponent* CapsuleComponent, FCollisionQueryParams QueryParams, bool bIsDebugEnabled,
	                  float DrawTime, FVector CharacterBottom, FHitResult& ForwardCheckHitResult);
	bool DownwardCheck(UCapsuleComponent* CapsuleComponent, FCollisionQueryParams QueryParams, bool bIsDebugEnabled,
	                   float DrawTime, FVector CharacterBottom, FHitResult ForwardCheckHitResult,
	                   FHitResult& DownwardCheckHitResult);
	bool OverlapCheck(UCapsuleComponent* CapsuleComponent, FCollisionQueryParams QueryParams, bool bIsDebugEnabled,
	                  float DrawTime, FHitResult DownwardCheckHitResult, FVector& OverlapLocation);

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Detection Settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float MinimumLedgeHeight = 40.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Detection Settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float MaximumLedgeHeight = 200.f;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Detection Settings", meta = (UIMin = 0.0f, ClampMin = 0.0f))
	float ForwardCheckDistance = 100.f;

private:
	TWeakObjectPtr<class ACharacter> CachedCharacterOwner;
};
