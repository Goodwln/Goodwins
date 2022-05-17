// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuWidget.h"

#include "Components/Button.h"
#include "Components/HorizontalBox.h"
#include "Kismet/GameplayStatics.h"
#include "Homework/BaseGameInstance.h"
#include "UI/Widget/LevelItemWidget.h"

DEFINE_LOG_CATEGORY_STATIC(LogMenuWidget, All, All);

void UMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if(IsValid(StartGameButton))
	{
		StartGameButton->OnClicked.AddDynamic(this, &UMenuWidget::OnStartGame);
	}

	if(IsValid(QuitGameButton))
	{
		QuitGameButton->OnClicked.AddDynamic(this, &UMenuWidget::OnQuitGame);
	}

	InitLevelItems();
}


void UMenuWidget::InitLevelItems()
{
	UBaseGameInstance* GameInstance = GetBaseGameInstance();
	if(!IsValid(GameInstance))
	{
		return;
	}

	checkf(GameInstance->GetLevelData().Num() != 0, TEXT("Levels data must not be empty!"));

	if(!IsValid(LevelItemsBox))
	{
		return;
	}
	LevelItemsBox->ClearChildren();

	for(auto LevelData : GameInstance->GetLevelData())
	{
		ULevelItemWidget* LevelItemWidget = CreateWidget<ULevelItemWidget>(GetWorld(),LevelItemWidgetClass);
		if(!IsValid(LevelItemWidget))
		{
			continue;
		}

		LevelItemWidget->SetLevelData(LevelData);
		LevelItemWidget->OnLevelSelectedEvent.AddUObject(this, &UMenuWidget::OnLevelSelected);

		LevelItemsBox->AddChild(LevelItemWidget);
		LevelItemWidgets.Add(LevelItemWidget);
	}

	if(GameInstance->GetStartupLevel().LevelName.IsNone())
	{
		OnLevelSelected(GameInstance->GetLevelData()[0]);
	}
	else
	{
		OnLevelSelected(GameInstance->GetStartupLevel());
	}
}

void UMenuWidget::OnLevelSelected(const FLevelData& Data)
{
	UBaseGameInstance* GameInstance = GetBaseGameInstance();
	if(!IsValid(GameInstance))
	{
		return;
	}

	GameInstance->SetStartupLevel(Data);

	for(auto LevelItemWidget: LevelItemWidgets)
	{
		if(IsValid(LevelItemWidget))
		{
			const bool bIsSelected = Data.LevelName == LevelItemWidget->GetLevelData().LevelName;
			LevelItemWidget->SetSelected(bIsSelected);
		}
	}
}

void UMenuWidget::OnStartGame()
{
	if(!IsValid(GetWorld()))
	{
		return;
	}

	const UBaseGameInstance* GameInstance = GetBaseGameInstance();
	if(!IsValid(GameInstance))
	{
		return;
	}
 
	UGameplayStatics::OpenLevel(this, GameInstance->GetStartupLevel().LevelName);
}

void UMenuWidget::OnQuitGame()
{
	if(!IsValid(GetWorld()))
	{
		return;
	}

	UKismetSystemLibrary::QuitGame(this, GetOwningPlayer(), EQuitPreference::Quit, true);
}


UBaseGameInstance* UMenuWidget::GetBaseGameInstance() const
{
	if(!IsValid(GetWorld()))
	{
		return nullptr;
	}

	return GetWorld()->GetGameInstance<UBaseGameInstance>();
}
