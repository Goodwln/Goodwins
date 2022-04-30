// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AIPatrollingComponent.generated.h"

UENUM()
enum class EPatrollingPathChange : uint8
{
	None,
	Circle,
	PingPong
};

class APatrollingPath;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK_API UAIPatrollingComponent : public UActorComponent
{
	GENERATED_BODY()
public:

	bool CanPatrol() const;
	FVector GetClosestWayPoint();
	
	void SetPatrollingPath(APatrollingPath* PatrollingPath_In) { PatrollingPath = PatrollingPath_In; }
	APatrollingPath* GetPatrollingPath() const { return PatrollingPath; }
	
	void SetPatrollingPathMode(EPatrollingPathChange NewPatrollingPathMode) { PatrollingPathChange = NewPatrollingPathMode; }
	
	FVector SelectNextWaypoint();
protected:
	
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Path")
	APatrollingPath* PatrollingPath;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Path")
	EPatrollingPathChange PatrollingPathChange = EPatrollingPathChange::None;
private:
	FVector SelectNextWaypointCircleMode();
	FVector SelectNextWaypointPingPongMode();
	int32 CurrentWayPointIndex = -1;
	bool MoveForward = true;
		
};
