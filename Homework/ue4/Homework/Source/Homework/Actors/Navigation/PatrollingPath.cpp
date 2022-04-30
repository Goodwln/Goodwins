// Fill out your copyright notice in the Description page of Project Settings.


#include "PatrollingPath.h"

APatrollingPath::APatrollingPath()
{
	bEnablePath = true;
}

const TArray<FVector>& APatrollingPath::GetWayPoint() const
{
	return WayPoint;
}

void APatrollingPath::SetEnablePath(bool bNewEnablePath)
{
	bEnablePath = bNewEnablePath;
}

bool APatrollingPath::IsEnablePath() const
{
	return bEnablePath; 
}
