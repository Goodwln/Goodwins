// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseUserWidget.h"
#include "GamePauseWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UGamePauseWidget : public UBaseUserWidget
{
	GENERATED_BODY()
 
public:
	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(meta = (BindWidget))
	class UButton* ClearPushButton;

private:
	UFUNCTION()
	void OnClearPause();
	
};
