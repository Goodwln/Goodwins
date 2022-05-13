// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuPlayerController.h"

#include "LearningGameInstance.h"

void AMenuPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeUIOnly());
	bShowMouseCursor = true;

	//GetWorld()->GetGameInstance<ULearningGameInstance>()->TestString = "Menu level says Hello";
}
