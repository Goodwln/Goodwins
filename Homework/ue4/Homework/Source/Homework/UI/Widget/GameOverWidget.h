// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkTypes.h"
#include "UI/Widget/BaseUserWidget.h"
#include "GameOverWidget.generated.h"


	class UVerticalBox;
	class UButton;
UCLASS()
class HOMEWORK_API UGameOverWidget : public UBaseUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* PlayerStatBox;

	UPROPERTY(meta = (BindWidget))
	UButton* ResetLevelButton;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UUserWidget> PlayerStatRowWidgetClass;

	virtual void NativeOnInitialized() override;
private:

	UFUNCTION()
	void OnResetLevel();
	
	void OnMatchStateChanged(EMatchState State);
	void UpdatePlayersStat();
	
};
