// Fill out your copyright notice in the Description page of Project Settings.


#include "GameOverWidget.h"

#include "GameModeLearning.h"
#include "PlayerStatRowWidget.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/HWPlayerState.h"
#include "Utils/HomeworkUtils.h"
 

void UGameOverWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	AGameModeLearning* GameMode = Cast<AGameModeLearning>(GetWorld()->GetAuthGameMode());
	if(IsValid(GameMode))
	{
		GameMode->OnMatchStateChangedEvent.AddUObject(this, &UGameOverWidget::OnMatchStateChanged);
	}

	if(IsValid(ResetLevelButton))
	{
		ResetLevelButton->OnClicked.AddDynamic(this, &UGameOverWidget::OnResetLevel);
	}
}

void UGameOverWidget::OnMatchStateChanged(EMatchState State)
{
	if(State == EMatchState::GameOver)
	{
		UpdatePlayersStat();
	}
}

void UGameOverWidget::UpdatePlayersStat()
{
	if(!IsValid(GetWorld()) || !IsValid(PlayerStatBox))
	{
		return;
	}

	PlayerStatBox->ClearChildren();
	
	for(auto It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		const AController* Controller = It->Get();
		if(!IsValid(Controller))
		{
			continue;
		}

		const AHWPlayerState* PlayerState = Controller->GetPlayerState<AHWPlayerState>();
		if(!IsValid(PlayerState))
		{
			continue;
		}

		UPlayerStatRowWidget* PlayerStatRowWidget = CreateWidget<UPlayerStatRowWidget>(GetWorld(), PlayerStatRowWidgetClass);
		if(!IsValid(PlayerStatRowWidget))
		{
			continue;
		}

		PlayerStatRowWidget->SetPlayerName(FText::FromString(PlayerState->GetPlayerName()));
		PlayerStatRowWidget->SetKills(HomeworkUtils::TextFromInt(PlayerState->GetKillsNum()));
		PlayerStatRowWidget->SetDeaths(HomeworkUtils::TextFromInt(PlayerState->GetDeathNum()));
		PlayerStatRowWidget->SetTeam(HomeworkUtils::TextFromInt(PlayerState->GetTeamID()));
		PlayerStatRowWidget->SetPlayerIndicatorVisibility(Controller->IsPlayerController());

		PlayerStatBox->AddChild(PlayerStatRowWidget);
	}
}


void UGameOverWidget::OnResetLevel()
{
	const FString CurrentLevelName = UGameplayStatics::GetCurrentLevelName(this);
	UGameplayStatics::OpenLevel(this, FName(CurrentLevelName));
}

