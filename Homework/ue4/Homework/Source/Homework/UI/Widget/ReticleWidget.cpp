// Fill out your copyright notice in the Description page of Project Settings.


#include "ReticleWidget.h"
#include "Actors/Equipment/EqupableItem.h"

void UReticleWidget::OnEquipItemChanged_Implementation(const AEqupableItem* EquipableItem)
{
	CurrentEquipableItem = EquipableItem;
	SetupCurrentReticle();

}

void UReticleWidget::OnAimingStateChanged_Implementation(bool bIsAiming)
{
	SetupCurrentReticle();
}

void UReticleWidget::SetupCurrentReticle()
{
	CurrentReticle = CurrentEquipableItem.IsValid() ? CurrentEquipableItem->GetCurrentReticle() : ECurrentReticle::None;
}
