// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkTypes.h"
#include "PlayerHUDWidget.h"
 #include "GameHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UGameHUDWidget : public UBaseUserWidget
{
	GENERATED_BODY()

public:
	virtual void InitializeWidget() override;
	virtual bool Initialize() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UPlayerHUDWidget> PlayerHUDWidgetClass;
	UPlayerHUDWidget* GetPlayerHUDWidget();
private:

	UPROPERTY()
	TMap<EMatchState, UBaseUserWidget*> MatchWidgets;
	
	UPROPERTY()
	FName PlayerHUDWidgetName = "UserWidget"; 
	
	void MatchStateChanged(EMatchState MatchState);

	
	void OnNewPawn(APawn* NewPawn);
};
