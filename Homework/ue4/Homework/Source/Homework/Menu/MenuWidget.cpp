// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

void UMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if(IsValid(StartGameButton))
	{
		StartGameButton->OnClicked.AddDynamic(this, &UMenuWidget::OnStartGame);
	}
}

void UMenuWidget::OnStartGame()
{
	const FName StartupLevelName = "Gym_Default";
	UGameplayStatics::OpenLevel(this, StartupLevelName);
}
