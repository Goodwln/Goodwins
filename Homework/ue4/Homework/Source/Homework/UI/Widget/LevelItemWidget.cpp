// Fill out your copyright notice in the Description page of Project Settings.


#include "LevelItemWidget.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"


void ULevelItemWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if(IsValid(LevelSelectButton))
	{
		LevelSelectButton->OnClicked.AddDynamic(this, &ULevelItemWidget::OnLevelItemClicked);
	}
}

void ULevelItemWidget::OnLevelItemClicked()
{
	OnLevelSelectedEvent.Broadcast(LevelData);
}

void ULevelItemWidget::SetLevelData(const FLevelData& Data)
{
	LevelData = Data;

	if(IsValid(LevelNameTextBlock))
	{
		LevelNameTextBlock->SetText(FText::FromName(Data.LevelDisplayName));
	}

	if(IsValid(LevelImage))
	{
		LevelImage->SetBrushFromTexture(Data.LevelThumb);
	}
}

void ULevelItemWidget::SetSelected(bool bIsSelected)
{
	if(IsValid(FrameImage))
	{
		FrameImage->SetVisibility(bIsSelected ? ESlateVisibility::Visible : ESlateVisibility::Hidden);
	}
}