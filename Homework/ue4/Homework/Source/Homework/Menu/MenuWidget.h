// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/BaseUserWidget.h"
#include "MenuWidget.generated.h"


class UButton;
class UHorizontalBox;
class UBaseGameInstance;
class ULevelItemWidget;


UCLASS()
class HOMEWORK_API UMenuWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* StartGameButton;
	
	UPROPERTY(meta = (BindWidget))
	UButton* QuitGameButton;

	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* LevelItemsBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UBaseUserWidget> LevelItemWidgetClass;
	
	virtual void NativeOnInitialized() override;

private:
	UPROPERTY()
	TArray<ULevelItemWidget*> LevelItemWidgets;
	
	UFUNCTION()
	void OnStartGame();

	UFUNCTION()
	void OnQuitGame();

	void InitLevelItems();

	void OnLevelSelected(const struct FLevelData& Data);
	UBaseGameInstance* GetBaseGameInstance() const;
};
