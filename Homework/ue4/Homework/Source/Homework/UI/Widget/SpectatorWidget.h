// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SpectatorWidget.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnVisibilitySpectatorWidgetEventSignature, bool);
class UTextBlock;
UCLASS()
class HOMEWORK_API USpectatorWidget : public UUserWidget
{
	GENERATED_BODY()
public:
	virtual void PostLoad() override;
	FOnVisibilitySpectatorWidgetEventSignature OnVisibilitySpectatorWidgetEvent;
protected:
	UFUNCTION()
	void TimeRespawnInfo(int32 CountDownTime);

	UFUNCTION()
	void VisibilityWidget(bool IsVisibility);
	
private:
	UPROPERTY(VisibleAnywhere ,Category = "SpectatorWidget", meta =(BindWidget))
	UTextBlock* TimeToRespawn;
};

