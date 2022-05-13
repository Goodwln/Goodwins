#pragma once
#include "PlayerState/HWPlayerState.h"


class HomeworkUtils
{
public:

	bool static AreEnemies(AController* FirstController, AController* SecondController)
	{
		if(!IsValid(FirstController) || !IsValid(SecondController) || FirstController == SecondController)
		{
			return false;
		}

		const AHWPlayerState* FirstPlayerState = FirstController->GetPlayerState<AHWPlayerState>();
		const AHWPlayerState* SecondPlayerState = SecondController->GetPlayerState<AHWPlayerState>();

		return IsValid(FirstPlayerState) && IsValid(SecondPlayerState) && FirstPlayerState->GetTeamID() != SecondPlayerState->GetTeamID() && !SecondPlayerState->IsDead();
	}

	static FText TextFromInt(int32 Number)
	{
		return FText::FromString(FString::FromInt(Number));
	}
};
