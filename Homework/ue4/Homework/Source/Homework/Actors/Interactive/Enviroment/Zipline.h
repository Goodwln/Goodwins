// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../InteractiveActor.h"
#include "Zipline.generated.h"

class UStaticMeshComponent;
class UCapsuleComponent;
/**
 * 
 */
UCLASS(Blueprintable)
class HOMEWORK_API AZipline : public AInteractiveActor
{
	GENERATED_BODY()
public:
	AZipline();
	float GetCableLength() const;
	const UStaticMeshComponent* GetCable() const;
	const FRotator GetDirectionCable() const;
	FVector GetFirstPillarLocation() const;
	FVector GetSecondPillarLocation() const;
	const UStaticMeshComponent* GetHighPillar() const;
	const UStaticMeshComponent* GetLowPillar() const;
	const FVector GetCenterBetweenPillars() const;
	const FRotator GetDirectionCableStatic() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category = "Zipline Parametrs")
	UStaticMeshComponent* FirstPillar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zipline Parametrs")
	UStaticMeshComponent* SecondPillar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zipline Parametrs")
	UStaticMeshComponent* Cable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zipline Parametrs")
	float FirstPillarOffset = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Zipline Parametrs")
	float SecondPillarOffset = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline Parametrs", meta = (MakeEditWidget))
	FVector FirstPillarLocation;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Zipline Parametrs", meta = (MakeEditWidget))
	FVector SecondPillarLocation;
	
	UCapsuleComponent* GetZiplineInteractionCapsule() const;
	virtual void OnConstruction(const FTransform& Transform) override;

	float FindPillarHalfHight(const UStaticMeshComponent* Pillar);

	void BeginPlay() override;
	
private:
	UPROPERTY()
	float MaxDistance = 0.f;
	
	UPROPERTY()
	FVector CenterBetweenPillars = FVector::ZeroVector;

	FRotator CableDirection = FRotator::ZeroRotator;
};


