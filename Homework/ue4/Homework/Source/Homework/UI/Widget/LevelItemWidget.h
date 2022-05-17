 // Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
 #include "UI/Widget/BaseUserWidget.h"
#include "LevelItemWidget.generated.h"
USTRUCT(BlueprintType)
struct FLevelData
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
	FName LevelName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
	FName LevelDisplayName = NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Level")
	UTexture2D* LevelThumb;
};

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelSelectedEventSignature, const FLevelData&);

class UButton;
class UTextBlock;
class UImage;

UCLASS()
class HOMEWORK_API ULevelItemWidget : public UBaseUserWidget
{
	GENERATED_BODY()

public:
	FOnLevelSelectedEventSignature OnLevelSelectedEvent;

	void SetLevelData(const FLevelData& Data);
	FLevelData GetLevelData() const { return LevelData; }

	void SetSelected(bool bIsSelected);
protected:
	UPROPERTY(meta =(BindWidget))
	UButton* LevelSelectButton;

	UPROPERTY(meta =(BindWidget))
	UTextBlock* LevelNameTextBlock;

	UPROPERTY(meta =(BindWidget))
	UImage* LevelImage;

	UPROPERTY(meta =(BindWidget))
	UImage* FrameImage;

	virtual void NativeOnInitialized() override;
private:
	FLevelData LevelData;

	UFUNCTION()
	void OnLevelItemClicked();
	
};
