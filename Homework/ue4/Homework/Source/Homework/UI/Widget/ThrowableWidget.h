// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ThrowableWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UThrowableWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void CapacityThrowable(int32 OutCurrentCapacity, int32 OutMaxThrowable);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Throwable")
	int32 CurrentCapacity = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Throwable")
	int32 MaxThrowable = 0;
};
