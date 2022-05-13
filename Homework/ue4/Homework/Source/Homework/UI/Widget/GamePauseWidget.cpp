// Fill out your copyright notice in the Description page of Project Settings.


#include "GamePauseWidget.h"
#include "GameFramework/GameModeBase.h"
#include "Components/Button.h"

void UGamePauseWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if(ClearPushButton)
	{
		ClearPushButton->OnClicked.AddDynamic(this, &UGamePauseWidget::OnClearPause);
	}
}

void UGamePauseWidget::OnClearPause()
{
	if(!IsValid(GetWorld()) || !IsValid(GetWorld()->GetAuthGameMode()))
	{
		return;
	}

	GetWorld()->GetAuthGameMode()->ClearPause();	
}
