// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseUserWidget.h"
#include "GoToMenuWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UGoToMenuWidget : public UBaseUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeOnInitialized() override;

	UPROPERTY(meta = (BindWidget))
	class UButton* GotoMenuButton;
private:

	UFUNCTION()
	void OnExecGotoMenuButton();
};
