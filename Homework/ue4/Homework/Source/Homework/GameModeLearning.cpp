// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModeLearning.h"

#include "AIController.h"
#include "EngineUtils.h"
 #include "Actors/DeadPlayer/DeadPlayer.h"
 #include "Characters/BaseCharacter.h"
#include "Characters/Controllers/BaseCharacterPlayerController.h"
 #include "Components/CharacterComponents/RespawnComponent.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerState/HWPlayerState.h"

DEFINE_LOG_CATEGORY_STATIC(LogGameModeLearning, All, All);

constexpr static int32 MinRoundTimeForRespawn = 10;

AGameModeLearning::AGameModeLearning()
{
	DefaultPawnClass = ABaseCharacter::StaticClass();
	PlayerControllerClass = ABaseCharacterPlayerController::StaticClass();
	PlayerStateClass = AHWPlayerState::StaticClass();
}

UClass* AGameModeLearning::GetDefaultPawnClassForController_Implementation(AController* InController)
{
	 if(InController && InController->IsA<AAIController>())
	 {
	 	return AIPawnClass;
	 }
		return Super::GetDefaultPawnClassForController_Implementation(InController);
}


void AGameModeLearning::StartPlay()
{
	Super::StartPlay();


	// UE_LOG(LogGameModeLearning, Display,TEXT("%s"), *GetWorld()->GetGameInstance<ULearningGameInstance>()->TestString);
	SpawnBots();
	CreateTeamsInfo();
	
	OnRoundTimeEvent.Broadcast(GameData.RoundTime);
	CurrentRound = 1;
	StartRound();

	SetMatchState(EMatchState::InProgress);
}

bool AGameModeLearning::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
	const bool bMatchState = Super::SetPause(PC, CanUnpauseDelegate);

	if(bMatchState)
	{
		SetMatchState(EMatchState::Pause);
	}
	
	return bMatchState;
}

bool AGameModeLearning::ClearPause()
{
	const bool bClearPause = Super::ClearPause();
	
	if(bClearPause)
	{
		SetMatchState(EMatchState::InProgress);
	}
	
	return bClearPause;
}

void AGameModeLearning::SpawnBots()
{
	if(!GetWorld())
	{
		return;
	}

	for(int32 i = 0; i < GameData.PlayerNum - 1; i++)
	{
		FActorSpawnParameters SpawnInfo;
		SpawnInfo.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	 	AController* AIController =GetWorld()->SpawnActor<AAIController>(AIControllerClass, SpawnInfo);
		
		RestartPlayer(AIController);
	}
}

void AGameModeLearning::StartRound()
{
	
	RoundCountDown = GameData.RoundTime;
	OnRoundTimeEvent.Broadcast(RoundCountDown--);
	OnRoundInfoEvent.Broadcast(CurrentRound, GameData.RoundsNum);
	GetWorldTimerManager().SetTimer(GameRoundTimerHandle, this, &AGameModeLearning::GameTimerUpdate, 1.f,true);
}

void AGameModeLearning::GameTimerUpdate()
{	
	OnRoundTimeEvent.Broadcast(RoundCountDown);
	if(RoundCountDown-- == 0)
	{
 		GetWorldTimerManager().ClearTimer(GameRoundTimerHandle);
		if(CurrentRound < GameData.RoundsNum)
		{
			++CurrentRound;
			ResetPlayers();
 			StartRound();
		}
		else
		{
			GameOver();

		}
	}
}

void AGameModeLearning::ResetPlayers()
{
	if(!GetWorld())
	{
		return;
	}
	CorpseCleaner();
	for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		ResetOnePlayer(It->Get());
	}
}
void AGameModeLearning::ResetOnePlayer(AController* Controller)
{
	if(Controller && Controller->GetPawn())
	{
		AHWPlayerState* PlayerState = Controller->GetPlayerState<AHWPlayerState>();
		if(IsValid(PlayerState))
		{
			PlayerState->SetDead(false);
		}
		
		Controller->GetPawn()->Reset();
	}
	
	RestartPlayer(Controller);
	SetPlayerColor(Controller);
}

void AGameModeLearning::CorpseCleaner()
{
	if(!IsValid(GetWorld()))
	{
		return;
	}

	TArray<AActor*> AllCorpse;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADeadPlayer::StaticClass(), AllCorpse);
	for(auto* Corpse : AllCorpse)
	{
		if(IsValid(Corpse))
		{
			Corpse->Destroy();
		}
	}
}

void AGameModeLearning::CreateTeamsInfo()
{
	if(!IsValid(GetWorld()))
	{
		return;
	}
	
	int32 TeamID = 1;
	for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
	{
 
		AController* Controller = IsValid(It->Get()) ? It->Get() : [ ]()->AController*{ return nullptr; }() ;
		if(!IsValid(Controller))
		{
			continue;
		}

		AHWPlayerState* PlayerState = Controller->GetPlayerState<AHWPlayerState>();
		if(!IsValid(PlayerState))
		{
			continue;
		}

		PlayerState->SetTeamID(TeamID);
		PlayerState->SetTeamColor(DetermineColorByTeamID(TeamID));
		PlayerState->SetPlayerName(Controller->IsPlayerController() ? "Player" : "Bot");
		SetPlayerColor(Controller);
		TeamID = TeamID == 1 ? 2 : 1;
	}
}

FLinearColor AGameModeLearning::DetermineColorByTeamID(int32 TeamID) const
{
	if(TeamID - 1 < GameData.TeamColors.Num())
	{
		return GameData.TeamColors[TeamID - 1];
	}
	UE_LOG(LogGameModeLearning, Warning, TEXT("No Color for team id: %i, set to Default color: %s"), TeamID, *GameData.DefaultTeamColor.ToString());
	return GameData.DefaultTeamColor;
}

void AGameModeLearning::SetPlayerColor(AController* Controller)
{
	if(!IsValid(Controller))
	{
		return;
	}

	ABaseCharacter* Character = Cast<ABaseCharacter>(Controller->GetPawn());
	if(!IsValid(Character))
	{
		return;
	}
	const AHWPlayerState* PlayerState = Controller->GetPlayerState<AHWPlayerState>();
	if(!IsValid(PlayerState))
	{
		return;
	}

	Character->SetPlayerColor(PlayerState->GetTeamColor());
}

void AGameModeLearning::Killed(AController* KillerController, AController* VictimController)
{
	AHWPlayerState* KillerPlayerState = IsValid(KillerController) ? KillerController->GetPlayerState<AHWPlayerState>() : nullptr;
	AHWPlayerState* VictimPlayerState = IsValid(VictimController) ? VictimController->GetPlayerState<AHWPlayerState>() : nullptr;

	if(IsValid(KillerPlayerState))
	{
		KillerPlayerState->AddKill();
 	}

	if(IsValid(VictimPlayerState))
	{
		VictimPlayerState->AddDeath();
		VictimPlayerState->SetDead(true);
 	}

	StartRespawn(VictimController);
	OnStatsPlayerEvent.Broadcast(KillerController, VictimController, IsValid(KillerPlayerState) ? KillerPlayerState->GetKillsNum() : -1, VictimPlayerState->GetDeathNum());
}


void AGameModeLearning::LogPlayerInfo()
{

	if(!IsValid(GetWorld()))
	{
		return;
	}
 
	for (auto It = GetWorld()->GetControllerIterator(); It; ++It)
	{
		AController* Controller = IsValid(It->Get()) ? It->Get() : [ ]()->AController*{ return nullptr; }() ;
		if(!IsValid(Controller))
		{
			continue;
		}

		AHWPlayerState* PlayerState = Controller->GetPlayerState<AHWPlayerState>();
		if(!IsValid(PlayerState))
		{
			continue;
		}
 	}
}

void AGameModeLearning::StartRespawn(AController* Controller)
{
	const bool bWasCurrentTimeForRespawn = RoundCountDown > MinRoundTimeForRespawn + GameData.RespawnTime;
	if(!bWasCurrentTimeForRespawn)
	{
		return;
	}
	
	URespawnComponent* RespawnComponent = Controller->FindComponentByClass<URespawnComponent>();
	if(!IsValid(RespawnComponent))
	{
		return;
	}

	RespawnComponent->Respawn(GameData.RespawnTime);
}

void AGameModeLearning::RespawnRequest(AController* Controller)
{
	ResetOnePlayer(Controller);
}

void AGameModeLearning::GameOver()
{
	UE_LOG(LogGameModeLearning, Display, TEXT("================== GAME OVER =================="));
	LogPlayerInfo();

	for(APawn* Pawn : TActorRange<APawn>(GetWorld()))
	{
		Pawn->TurnOff();
		Pawn->DisableInput(nullptr);
	}

	SetMatchState(EMatchState::GameOver);
}

void AGameModeLearning::SetMatchState(EMatchState NewState)
{
	if(MatchState == NewState)
	{
		return;
	}
	MatchState = NewState;
	OnMatchStateChangedEvent.Broadcast(MatchState);
	
}

