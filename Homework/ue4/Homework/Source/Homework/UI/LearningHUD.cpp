// Fill out your copyright notice in the Description page of Project Settings.


#include "LearningHUD.h"

#include "GameModeLearning.h"
#include "Engine/Canvas.h"
#include "Widget/PlayerHUDWidget.h"
#include "Widget/GamePauseWidget.h"

void ALearningHUD::DrawHUD()
{
	Super::DrawHUD();

//	DrawCrossHair();
}

void ALearningHUD::BeginPlay()
{
	Super::BeginPlay();

	 
	MatchWidgets.Add(EMatchState::InProgress, CreateWidget<UBaseUserWidget>(GetWorld(),PlayerHUDWidgetClass));
	MatchWidgets.Add(EMatchState::Pause, CreateWidget<UBaseUserWidget>(GetWorld(),GamePauseWidgetClass));
	MatchWidgets.Add(EMatchState::GameOver, CreateWidget<UBaseUserWidget>(GetWorld(),GameOverWidgetClass));

	for(auto& Elem : MatchWidgets)
	{
		UBaseUserWidget* Widget = Elem.Value;
		if(!IsValid(Widget))
		{
			continue;
		}

		Widget->AddToViewport();
		Widget->InitializeWidget();
		Widget->SetVisibility(ESlateVisibility::Hidden);
	}

	AGameModeLearning* GameMode = Cast<AGameModeLearning>(GetWorld()->GetAuthGameMode());
	if(IsValid(GameMode))
	{
		GameMode->OnMatchStateChangedEvent.AddUObject(this, &ALearningHUD::MatchStateChanged);
	}
}

void ALearningHUD::DrawCrossHair()
{
	const TInterval<float> Center(Canvas->SizeX * 0.5f, Canvas->SizeY * 0.5f);

	const float HalfLineSize = 10.f;
	const float LineThickness = 2.f;
	const FLinearColor LinearColor = FLinearColor::Green;

	DrawLine(Center.Min - HalfLineSize, Center.Max, Center.Min + HalfLineSize, Center.Max, LinearColor, LineThickness);
	DrawLine(Center.Min, Center.Max - HalfLineSize, Center.Min, Center.Max + HalfLineSize, LinearColor, LineThickness);
}

void ALearningHUD::MatchStateChanged(EMatchState MatchState)
{
	if(CurrentWidget)
	{
		CurrentWidget->SetVisibility(ESlateVisibility::Hidden);
	}

	if(MatchWidgets.Contains(MatchState))
	{
		CurrentWidget = MatchWidgets.FindRef(MatchState);
	}

	if(IsValid(CurrentWidget))
	{
		CurrentWidget->SetVisibility(ESlateVisibility::Visible);
	}
	
}
