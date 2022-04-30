// Fill out your copyright notice in the Description page of Project Settings.


#include "SpectatorWidget.h"

#include "Components/TextBlock.h"


void USpectatorWidget::PostLoad()
{
	Super::PostLoad();

	SetVisibility(ESlateVisibility::Hidden);
}

void USpectatorWidget::TimeRespawnInfo(int32 CountDownTime)
{
	if(CountDownTime < 10)
	{
		const FText Text = FText::FromString("0" + FString::FromInt(CountDownTime));
		TimeToRespawn->SetText(Text);
		return;
	}
	const FText Text = FText::FromString(FString::FromInt(CountDownTime));
	TimeToRespawn->SetText(Text);
}

void USpectatorWidget::VisibilityWidget(bool IsVisibility)
{
	OnVisibilitySpectatorWidgetEvent.Broadcast(IsVisibility);
	ESlateVisibility CurrentVisibility = IsVisibility ? ESlateVisibility::Visible : ESlateVisibility::Hidden;
	SetVisibility(CurrentVisibility);
}
 