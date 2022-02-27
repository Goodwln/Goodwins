// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkTypes.h"
#include "Blueprint/UserWidget.h"
#include "ReticleWidget.generated.h"

class AEqupableItem;

UCLASS(BlueprintType)
class HOMEWORK_API UReticleWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UFUNCTION(BlueprintNativeEvent)
	void OnAimingStateChanged(bool bIsAiming);

	UFUNCTION(BlueprintNativeEvent)
	void OnEquipItemChanged(const AEqupableItem* EquipableItem);
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reticle")
	ECurrentReticle CurrentReticle = ECurrentReticle::None;

private:
	TWeakObjectPtr<const AEqupableItem> CurrentEquipableItem;

	void SetupCurrentReticle();
};
