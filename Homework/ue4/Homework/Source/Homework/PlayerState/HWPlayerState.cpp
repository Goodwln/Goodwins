// Fill out your copyright notice in the Description page of Project Settings.


#include "HWPlayerState.h"

DEFINE_LOG_CATEGORY_STATIC(LogPlayerStateHomework, All, All);
 

void AHWPlayerState::LogInfo()
{
	UE_LOG(LogPlayerStateHomework, Display, TEXT("Team: %i, Kills: %i, Deaths: %i"), TeamID, KillsNum, DeathNum);
}
