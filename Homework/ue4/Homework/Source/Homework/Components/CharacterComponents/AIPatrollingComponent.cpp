// Fill out your copyright notice in the Description page of Project Settings.


#include "AIPatrollingComponent.h"

#include "AIController.h"
#include "Homework/Actors/Navigation/PatrollingPath.h"

bool UAIPatrollingComponent::CanPatrol() const
{
	return IsValid(PatrollingPath) && PatrollingPath->GetWayPoint().Num() > 0;
}

FVector UAIPatrollingComponent::GetClosestWayPoint()
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	AController* Controller = Cast<AController>(GetOwner());
	if(IsValid(Controller) && IsValid(Controller->GetPawn()))
	{
		OwnerLocation = Controller->GetPawn()->GetActorLocation();
	}
	
	const TArray<FVector> WayPoints = PatrollingPath->GetWayPoint();
	FTransform PathTransform = PatrollingPath->GetActorTransform();

	FVector ClosestWayPoint;
	float MinSquareDistance = FLT_MAX;
	
	for(int32 i = 0; i < WayPoints.Num(); ++i)
	{
		FVector WayPointWorld = PathTransform.TransformPosition(WayPoints[i]);
		float CurrentSquareDistance = (OwnerLocation - WayPointWorld).SizeSquared();
		if(CurrentSquareDistance < MinSquareDistance)
		{
			MinSquareDistance = CurrentSquareDistance;
			ClosestWayPoint = WayPointWorld;
			CurrentWayPointIndex = i;
		}
	}
	
	return  ClosestWayPoint;

}

FVector UAIPatrollingComponent::SelectNextWaypointCircleMode()
{
	++CurrentWayPointIndex;
	if(CurrentWayPointIndex == PatrollingPath->GetWayPoint().Num())
	{
		CurrentWayPointIndex = 0;
	}

	FTransform PathTransform = PatrollingPath->GetActorTransform();
	const TArray<FVector> WayPoints = PatrollingPath->GetWayPoint();

	FVector WayPoint = PathTransform.TransformPosition(WayPoints[CurrentWayPointIndex]);

	return WayPoint;
}

FVector UAIPatrollingComponent::SelectNextWaypointPingPongMode()
{
	MoveForward ? ++CurrentWayPointIndex : --CurrentWayPointIndex;
	
	if(CurrentWayPointIndex == PatrollingPath->GetWayPoint().Num()) 
	{
		MoveForward = false;
		CurrentWayPointIndex = PatrollingPath->GetWayPoint().Num() - 2;
	}
	
	if(CurrentWayPointIndex == 0) MoveForward = true;

	FTransform PathTransform = PatrollingPath->GetActorTransform();
	const TArray<FVector> WayPoints = PatrollingPath->GetWayPoint();

	FVector WayPoint = PathTransform.TransformPosition(WayPoints[CurrentWayPointIndex]);

	return WayPoint;

	
}

FVector UAIPatrollingComponent::SelectNextWaypoint()
{
	FVector Result = FVector::ZeroVector;
	switch (PatrollingPathChange)
	{
		case EPatrollingPathChange::Circle:
			{
				Result = SelectNextWaypointCircleMode();
				break;
			}
		 case EPatrollingPathChange::PingPong:
		 	{
		 		Result = SelectNextWaypointPingPongMode();
		 		break;
		 	}
		 case EPatrollingPathChange::None:
		 	{
		 		Result = GetOwner()->GetActorLocation();
		 		break;
		 	}
	}
	
	return Result;
}


