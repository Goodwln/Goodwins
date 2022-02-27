// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "WidgetCharacterAttributes.generated.h"

struct FDefaultAttributeProperty;
enum class ECurrentAttribute : uint8;
/**
 * 
 */
UCLASS()
class HOMEWORK_API UWidgetCharacterAttributes : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void OnAttributeUpdate(float CurrentValue, float MaxValue, FDefaultAttributeProperty Attribute);

protected:

	UPROPERTY(BlueprintReadOnly, Category = "Attribute", meta=(BindWidget))
	class UProgressBar* AttributeProgressBar;
};
