// Fill out your copyright notice in the Description page of Project Settings.


#include "WidgetCharacterAttributes.h"

#include "Components/ProgressBar.h"
#include "Components/CharacterComponents/CharacterAttributeComponent.h"

void UWidgetCharacterAttributes::OnAttributeUpdate(float CurrentValue, float MaxValue, FDefaultAttributeProperty Attribute)
{
	if(!IsValid(AttributeProgressBar))
	{
		return;
	}
	
	const ESlateVisibility CurrentVisibility = (CurrentValue == MaxValue) ? ESlateVisibility::Hidden : ESlateVisibility::Visible;
	AttributeProgressBar->SetVisibility(CurrentVisibility);
	
	const float CurrentPercent = CurrentValue / MaxValue;
	AttributeProgressBar->SetPercent(CurrentPercent);
	
	AttributeProgressBar->SetFillColorAndOpacity(Attribute.Color);
}
