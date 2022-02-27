 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../InteractiveActor.h"
#include "../../BasePlatform.h"

#include "Ladder.generated.h"


class UInstancedStaticMeshComponent;
class UStaticMeshComponent;
class UAnimMontage;

/**
 * 
 */
UCLASS(Blueprintable)
class HOMEWORK_API ALadder : public AInteractiveActor
{
	GENERATED_BODY()


public:
	ALadder();

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	float GetLadderHeight() const;
	bool GetIsOnTop() const;
	UAnimMontage* GetAttachFromTopAnimMontage() const;
	FVector GetAttachFromTopAnimMontageStartingLocation() const;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	float LadderHeight = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	float LadderWidth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	float StepsInterval = 25.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder Parameters")
	float BottomStepsOffset = 25.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* RightRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* LeftRailMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UInstancedStaticMeshComponent* StepsMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	class UBoxComponent* TopInteractionVolume;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Ladder Parameters")
	UAnimMontage* AttachFromTopAnimMontage;

	//Offset from Ladder's top for starting anim montage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ladder parameters")
	FVector AttachFromTopAnimMontageInitialOffset = FVector::ZeroVector;
	
	class UBoxComponent* GetLadderInteractionBox() const;

	virtual void OnInteractionVolumeOverlabBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;
	virtual void OnInteractionVolumeOverlabEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex) override;

private:
	bool bIsOnTop = false;
};
