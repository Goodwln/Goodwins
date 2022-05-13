// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuHUD.h"

#include "MenuWidget.h"
#include "Blueprint/UserWidget.h"

void AMenuHUD::BeginPlay()
{
	Super::BeginPlay();

	if(IsValid(MenuWidgetClass))
	{
		UUserWidget* MenuWidget = CreateWidget<UMenuWidget>(GetWorld(), MenuWidgetClass);
		if(IsValid(MenuWidget))
		{
			MenuWidget->AddToViewport();
		}
	}
}
