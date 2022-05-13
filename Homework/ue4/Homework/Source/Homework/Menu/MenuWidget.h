// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MenuWidget.generated.h"


class UButton;

UCLASS()
class HOMEWORK_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* StartGameButton;
	
	virtual void NativeOnInitialized() override;

private:
	UFUNCTION()
	void OnStartGame();
};
