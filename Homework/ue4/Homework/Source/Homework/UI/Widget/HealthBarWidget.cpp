// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthBarWidget.h"

#include "Components/ProgressBar.h"

void UHealthBarWidget::SetHealthPercent(float Percent)
{
	if(!IsValid(HealthProgressBar))
	{
		return;
	}

	const ESlateVisibility HealthBarVisibility = Percent > PercentVisibilityThreshold || FMath::IsNearlyZero(Percent) //
													? ESlateVisibility::Hidden
													: ESlateVisibility::Visible;
	HealthProgressBar->SetVisibility(HealthBarVisibility);

	const FLinearColor HealthBarColor = Percent > PercentColorThreshold ? GoodColor : BadColor;
	HealthProgressBar->SetFillColorAndOpacity(HealthBarColor);

	HealthProgressBar->SetPercent(Percent);

}
