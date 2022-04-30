// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PatrollingPath.generated.h"

UCLASS()
class HOMEWORK_API APatrollingPath : public AActor
{
	GENERATED_BODY()

public:
	APatrollingPath();
	
	const TArray<FVector>& GetWayPoint() const;

	void SetEnablePath(bool bNewEnablePath);
	bool IsEnablePath() const;
protected:
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Path", meta = (MakeEditWidget))
	TArray<FVector> WayPoint;

private:
	UPROPERTY()
	bool bEnablePath;
};
