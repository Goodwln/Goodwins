// Fill out your copyright notice in the Description page of Project Settings.


#include "GoToMenuWidget.h"

#include "BaseGameInstance.h"
#include "Components/Button.h"
#include "Kismet/GameplayStatics.h"

DEFINE_LOG_CATEGORY_STATIC(LogGotoMenuWidget, All, All);

void UGoToMenuWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	if(IsValid(GotoMenuButton))
	{
		GotoMenuButton->OnClicked.AddDynamic(this, &UGoToMenuWidget::OnExecGotoMenuButton);
	}
}

void UGoToMenuWidget::OnExecGotoMenuButton()
{
	if(!IsValid(GetWorld()))
	{
		return;
	}

	const UBaseGameInstance* GameInstance = GetWorld()->GetGameInstance<UBaseGameInstance>();
	if(!IsValid(GameInstance))
	{
		return;
	}

	if(GameInstance->GetMenuLevelName().IsNone())
	{
		UE_LOG(LogGotoMenuWidget,Error, TEXT("Game instance haven't got a Menu level"));
		return;
	}

	UGameplayStatics::OpenLevel(this ,GameInstance->GetMenuLevelName());
}
