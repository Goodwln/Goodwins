// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RoundInfoWidget.generated.h"

class UTextBlock;

UCLASS()
class HOMEWORK_API URoundInfoWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void TimeInfo(int32 RoundTime);
	UFUNCTION()
	void KillsInfo(int32 KillsNum);
	UFUNCTION()
	void DeathsInfo(int32 DeadNum);
	UFUNCTION()
	void RoundInfo(int32 RoundNum, int32 RoundMax);
protected:
	UPROPERTY(BlueprintReadWrite, Category = "RoundInfo",meta = (BindWidget))
	UTextBlock* KillInfoText;
	UPROPERTY(BlueprintReadWrite, Category = "RoundInfo",meta = (BindWidget))
	UTextBlock* DeathInfoText;
	
	UPROPERTY(BlueprintReadOnly, Category = "RoundInfo",meta = (BindWidget))
	UTextBlock* SecondsTimeInfoText;
	UPROPERTY(BlueprintReadOnly, Category = "RoundInfo",meta = (BindWidget))
	UTextBlock* MinutesTimeInfoText;
	
	UPROPERTY(BlueprintReadOnly, Category = "RoundInfo",meta = (BindWidget))
	UTextBlock* CurrentRoundInfoText;
	UPROPERTY(BlueprintReadOnly, Category = "RoundInfo",meta = (BindWidget))
	UTextBlock* MaxRoundInfoText;
	
 
};
