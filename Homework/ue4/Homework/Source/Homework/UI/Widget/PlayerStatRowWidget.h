// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseUserWidget.h"
#include "PlayerStatRowWidget.generated.h"


class UImage;
class UTextBlock;

UCLASS()
class HOMEWORK_API UPlayerStatRowWidget : public UBaseUserWidget
{
	GENERATED_BODY()

public:
	void SetPlayerName(const FText& Text);
	void SetKills(const FText& Text);
	void SetDeaths(const FText& Text);
	void SetTeam(const FText& Text);
	void SetPlayerIndicatorVisibility(bool bVisible);
	
protected:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* PlayerNameTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* KillsTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* DeathsTextBlock;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* TeamTextBlock;

	UPROPERTY(meta = (BindWidget))
	UImage* PlayerIndicatorImage;
};
