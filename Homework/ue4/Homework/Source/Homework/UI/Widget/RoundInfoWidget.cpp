// Fill out your copyright notice in the Description page of Project Settings.


#include "RoundInfoWidget.h"

#include "Components/TextBlock.h"
 

void URoundInfoWidget::TimeInfo(int32 RoundTime)
{
	const int32 Seconds = RoundTime % 60;
	const int32 Minutes = RoundTime / 60;

	const FText SecondsText = FText::FromString(FString::FromInt(Seconds));
	const FText MinutesText = FText::FromString(FString::FromInt(Minutes));

	SecondsTimeInfoText->SetText(SecondsText);
	MinutesTimeInfoText->SetText(MinutesText);
}

void URoundInfoWidget::KillsInfo(int32 KillsNum)
{
	const FText Text = FText::FromString(FString::FromInt(KillsNum)); 
	KillInfoText->SetText(Text);
}

void URoundInfoWidget::DeathsInfo(int32 DeadNum)
{
	const FText Text = FText::FromString(FString::FromInt(DeadNum)); 
	DeathInfoText->SetText(Text);
}

void URoundInfoWidget::RoundInfo(int32 RoundNum, int32 RoundMax)
{
	const FText CurrentNum = FText::FromString(FString::FromInt(RoundNum));
	const FText MaxNum = FText::FromString(FString::FromInt(RoundMax));

	CurrentRoundInfoText->SetText(CurrentNum);
	MaxRoundInfoText->SetText(MaxNum);
}
