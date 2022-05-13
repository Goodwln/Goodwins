// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkTypes.h"
#include "GameFramework/HUD.h"

#include "LearningHUD.generated.h"

class UBaseUserWidget;
class UPlayerHUDWidget;
class UGamePauseWidget;
UCLASS()
class HOMEWORK_API ALearningHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;
	virtual void BeginPlay() override;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UBaseUserWidget> PlayerHUDWidgetClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UBaseUserWidget> GamePauseWidgetClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UBaseUserWidget> GameOverWidgetClass;
private:
	void DrawCrossHair();

	UPROPERTY()
	TMap<EMatchState, UBaseUserWidget*> MatchWidgets;

	UFUNCTION()
	void MatchStateChanged(EMatchState MatchState);

	UPROPERTY()
	UBaseUserWidget* CurrentWidget;
};
