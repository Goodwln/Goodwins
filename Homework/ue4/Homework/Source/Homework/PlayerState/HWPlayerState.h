// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "HWPlayerState.generated.h"
 
UCLASS()
class HOMEWORK_API AHWPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
 
	void SetTeamID(int32 NewTeamID) { TeamID = NewTeamID; }
	int32 GetTeamID() const { return TeamID; }

	void SetTeamColor(FLinearColor NewTeamColor) { TeamColor = NewTeamColor; }
	FLinearColor GetTeamColor() const { return TeamColor; }

	void AddKill() { ++KillsNum; }
	int32 GetKillsNum() const {return  KillsNum; }

	void AddDeath() { ++DeathNum; }
	int32 GetDeathNum() const {return  DeathNum; }

	void SetDead(bool bIsDead_In) { bIsDead = bIsDead_In; }
	bool IsDead() const { return bIsDead; }
	
	void LogInfo();
private:
	int32 TeamID;
	FLinearColor TeamColor;

	int32 KillsNum = 0;
	int32 DeathNum = 0;

	bool bIsDead = false;
};
