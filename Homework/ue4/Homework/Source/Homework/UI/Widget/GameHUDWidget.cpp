// Fill out your copyright notice in the Description page of Project Settings.


#include "GameHUDWidget.h"

#include "Blueprint/WidgetTree.h"

DEFINE_LOG_CATEGORY_STATIC(LogGameHUDWidget, All, All);

void UGameHUDWidget::InitializeWidget()
{
	UPlayerHUDWidget* PlayerHUDWidget = CreateWidget<UPlayerHUDWidget>(GetWorld(), PlayerHUDWidgetClass);
	if(IsValid(PlayerHUDWidget))
	{
		PlayerHUDWidget->InitializeWidget();
 	}
 
}


void UGameHUDWidget::OnNewPawn(APawn* NewPawn)
{

	InitializeWidget();

}

bool UGameHUDWidget::Initialize()
{
	if(GetOwningPlayer())
	{
		GetOwningPlayer()->GetOnNewPawnNotifier().AddUObject(this, &UGameHUDWidget::OnNewPawn);
	}
	
	return Super::Initialize();
}



UPlayerHUDWidget* UGameHUDWidget::GetPlayerHUDWidget()
{
	return WidgetTree->FindWidget<UPlayerHUDWidget>(PlayerHUDWidgetName);
}

void UGameHUDWidget::MatchStateChanged(EMatchState MatchState)
{
	UE_LOG(LogGameHUDWidget, Display, TEXT("Current match state: %s"), *UEnum::GetValueAsString(MatchState));

}
