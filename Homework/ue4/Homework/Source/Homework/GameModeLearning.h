// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkTypes.h"
#include "GameFramework/GameModeBase.h"
#include "GameModeLearning.generated.h"

USTRUCT(BlueprintType)
struct FGameData
{
	GENERATED_USTRUCT_BODY()

    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game",meta = (ClampMin = 1, ClampMax = 100))
	int32 PlayerNum = 2;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game",meta = (ClampMin = 1, ClampMax = 10))
	int32 RoundsNum = 4;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game",meta = (ClampMin = 3, ClampMax = 300))
	int32 RoundTime = 10; // In Seconds
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	FLinearColor DefaultTeamColor = FLinearColor::Red;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray<FLinearColor> TeamColors;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Game",meta = (ClampMin = 3, ClampMax = 20))
	int32 RespawnTime = 5.f;
};

class AAIController;
class AAICharacter;
class APatrollingPath;

DECLARE_MULTICAST_DELEGATE_FourParams(FOnStatsPlayerEventSignature, AController*, AController*, int32, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRoundInfoEventSignature, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRoundTimeEventSignature, int32);
 
UCLASS()
class HOMEWORK_API AGameModeLearning : public AGameModeBase
{
	GENERATED_BODY()

public:
	AGameModeLearning();

	FOnMatchStateChangedEventSignature OnMatchStateChangedEvent;
	
	virtual UClass* GetDefaultPawnClassForController_Implementation(AController* InController) override;
	virtual void StartPlay() override;
	virtual bool SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate = FCanUnpause()) override;
	virtual bool ClearPause() override;
	void Killed(AController* KillerController, AController* VictimController);
	void RespawnRequest(AController* Controller);

	FOnStatsPlayerEventSignature OnStatsPlayerEvent;
	FOnRoundInfoEventSignature OnRoundInfoEvent;
	FOnRoundTimeEventSignature OnRoundTimeEvent;
 
protected:

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	TSubclassOf<AAIController> AIControllerClass;

	UPROPERTY(EditDefaultsOnly, Category = "Game")
	TSubclassOf<APawn> AIPawnClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Game")
	FGameData GameData;
private:

	EMatchState MatchState = EMatchState::WaitingToStart;
	int32 CurrentRound = 0;
	int32 RoundCountDown = 0;
	FTimerHandle GameRoundTimerHandle;
	
	void SpawnBots();
	void StartRound();
	void GameTimerUpdate();

	void ResetPlayers();
	void ResetOnePlayer(AController* Controller);
	void CorpseCleaner();
	
	void CreateTeamsInfo();
	FLinearColor DetermineColorByTeamID(int32 TeamID) const;
	void SetPlayerColor(AController* Controller);
	
	void LogPlayerInfo();

	void StartRespawn(AController* Controller);

	void GameOver();

	void SetMatchState(EMatchState NewState);
	
};
