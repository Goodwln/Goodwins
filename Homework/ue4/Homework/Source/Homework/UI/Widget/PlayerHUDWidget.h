// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Characters/BaseCharacter.h"
#include "PlayerHUDWidget.generated.h"

/**
 * 
 */
class UWidgetCharacterAttributes;
class USpectatorWidget;
class UThrowableWidget;
class URoundInfoWidget;
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateKillStatsInfoEventSignature, int32 )
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateDeathStatsInfoEventSignature, int32 )
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnUpdateTimeRoundStatsInfoEventSignature, int32 )
	DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUpdateRoundInfoEventSignature, int32, int32 )

UCLASS()
class HOMEWORK_API UPlayerHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:

		virtual bool Initialize() override;
	void InitReticleWidget(ABaseCharacter* Character);
	void InitAmmoWidget(ABaseCharacter* Character);
	void InitStaminaWidget(ABaseCharacter* Character);
	void InitOxygenWidget(ABaseCharacter* Character);
	void InitHealthWidget(ABaseCharacter* Character);
	void InitThrowableWidget(ABaseCharacter* Character);
	void InitRoundInfoWidget();
	void InitSpectatorWidget();
	void CreateAndInitializeWidget(APawn* PawnOwner, AController* Controller);
		
		class UReticleWidget* GetReticleWidget();

		class UAmmoWidget* GetAmmoWidget();

		UWidgetCharacterAttributes* GetStaminaAttributeWidget();

		UWidgetCharacterAttributes* GetOxygenAttributeWidget();

		UWidgetCharacterAttributes* GetHealthAttributeWidget();
	
		  UThrowableWidget* GetThrowableWidget();

		URoundInfoWidget* GetRoundInfoWidget();

		USpectatorWidget* GetTimeRespawnInfoWidget();

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

		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names");
		FName RoundInfoWidgetName = "PlayerRoundInfo";
		
		UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget names");
		FName TimeRespawnInfoWidgetName = "TimeRespawnInfo";
	
private:
		UPROPERTY()
		AController* CacheController;
		
		UFUNCTION()
		void UpdatePlayerStats(AController*  KillerController, AController* VictimController, int32 Kills, int32 Deaths);

		UFUNCTION()
		void WasSpectatorMode(bool IsVisibility);
		
		FOnUpdateKillStatsInfoEventSignature OnUpdateKillStatsInfoEvent;
		FOnUpdateDeathStatsInfoEventSignature OnUpdateDeathStatsInfoEvent;

		void OnNewPawn(APawn* NewPawn);
};
