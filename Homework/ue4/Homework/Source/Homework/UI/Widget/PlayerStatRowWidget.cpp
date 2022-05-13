// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStatRowWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"

void UPlayerStatRowWidget::SetPlayerName(const FText& Text)
{
	if(!IsValid(PlayerNameTextBlock))
	{
		return;
	}
	
	PlayerNameTextBlock->SetText(Text);
}

void UPlayerStatRowWidget::SetKills(const FText& Text)
{
	if(!IsValid(KillsTextBlock))
	{
		return;
	}

	KillsTextBlock->SetText(Text);
	
}

void UPlayerStatRowWidget::SetDeaths(const FText& Text)
{
	if(!IsValid(DeathsTextBlock))
	{
		return;
	}

	DeathsTextBlock->SetText(Text);
}

void UPlayerStatRowWidget::SetTeam(const FText& Text)
{
	if(!IsValid(TeamTextBlock))
	{
		return;
	}

	TeamTextBlock->SetText(Text);
}

void UPlayerStatRowWidget::SetPlayerIndicatorVisibility(bool bVisible)
{
	if(!IsValid(PlayerIndicatorImage))
	{
		return;
	}

	PlayerIndicatorImage->SetVisibility(bVisible ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
}
