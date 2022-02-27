// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class HOMEWORK_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	
	void CreateAndInitializeWidget(class ABaseCharacter* CharacterOwner);
	
	class UReticleWidget* GetReticleWidget();
	
	class UAmmoWidget* GetAmmoWidget();

	class UWidgetCharacterAttributes* GetStaminaAttributeWidget();

	class UWidgetCharacterAttributes* GetOxygenAttributeWidget();

	class UWidgetCharacterAttributes* GetHealthAttributeWidget();
	
	class UThrowableWidget* GetThrowableWidget();
protected:
	
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names");
	FName ReticleWidgetName = "Reticle";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names");
	FName AmmoWidgetName = "WBP_Ammo";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names");
	FName StaminaAttributeWidgetName = "StaminaAttribute";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names");
	FName OxygenAttributeWidgetName = "OxygenAttribute";
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names");
	FName HealthAttributeWidgetName = "HealthAttribute";

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names");
	FName ThrowableWidgetName = "HealthAttribute";

};
