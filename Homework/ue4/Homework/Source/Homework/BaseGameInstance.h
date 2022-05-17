// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "UI/Widget/LevelItemWidget.h"
#include "BaseGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UBaseGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	virtual bool ProcessConsoleExec(const TCHAR* Cmd, FOutputDevice& Ar, UObject* Executor) override;

	FLevelData GetStartupLevel() const { return StartupLevel; }
	void SetStartupLevel(const FLevelData& Data) {StartupLevel = Data; }

	TArray<FLevelData> GetLevelData() const { return LevelsData; }
	FName GetMenuLevelName() const { return MenuLevelName; }
	
protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Game", meta = (ToolTip = "Level names must be unique!"))
	TArray<FLevelData> LevelsData;
 
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	FName MenuLevelName = NAME_None;
private:
	FLevelData StartupLevel;
};
