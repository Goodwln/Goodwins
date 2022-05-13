// Fill out your copyright notice in the Description page of Project Settings.


#include "MenuGameMode.h"

#include "MenuHUD.h"
#include "MenuPlayerController.h"

AMenuGameMode::AMenuGameMode()
{
	HUDClass = AMenuHUD::StaticClass();
	PlayerControllerClass = AMenuPlayerController::StaticClass();
}
