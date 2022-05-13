// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "LearningGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API ULearningGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	FString TestString = "Hello game!";
};
