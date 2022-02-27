// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterEquipmentComponent.h"

#include "HomeworkTypes.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Characters/BaseCharacter.h"


EEquipableItemType UCharacterEquipmentComponent::GetCurrentEquippedItemType() const
{
	EEquipableItemType Result = EEquipableItemType::None;

	if(IsValid(CurrentEquippedWeapon))
	{
		Result = CurrentEquippedWeapon->GetItemType();
	}
	return Result;
}

ARangeWeaponItem* UCharacterEquipmentComponent::GetCurrentRangeWeapon() const
{
	return CurrentEquippedWeapon;
}

void UCharacterEquipmentComponent::ReloadCurrentWeapon()
{
	check(IsValid(CurrentEquippedWeapon));

	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	if(AvailableAmunition <= 0)
	{
		return;
	}

	CurrentEquippedWeapon->StartReload();
}

void UCharacterEquipmentComponent::UnEquipCurrentItem()
{
	if(IsValid(CurrentEquipableItem))
	{
		CurrentEquipableItem->AttachToComponent(CacheBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquipableItem->GetUnEquippedSocketName());
	}

	if(IsValid(CurrentEquippedWeapon))
	{
		
		CurrentEquippedWeapon->StopFire();
		CurrentEquippedWeapon->EndReload(false);
		CurrentEquippedWeapon->OnAmmoChange.Remove(OnCurrentWeaponUpdatedAmmoChangedHandle);
		CurrentEquippedWeapon->OnReloadComplete.Remove(OnWeaponReloadCompleteHandle);
		OnCurrentWeaponUpdatedAmmoChanged(CurrentEquippedWeapon->GetAmmo());
	}
	
	CurrentEquipmentSlot = EEquipmentSlot::None;
	
	if(IsValid(CurrentEquipableItem))
	{
		CurrentEquipableItem->UnEquip();

	}

}

void UCharacterEquipmentComponent::AttachCurrentItemToEquippedSocket()
{
	if(IsValid(CurrentEquipableItem))
	{
		CurrentEquipableItem->AttachToComponent(CacheBaseCharacter->GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, CurrentEquipableItem->GetEquippedSocketName());
	}
}

void UCharacterEquipmentComponent::LaunchThrowCurrenThrowable()
{
	if(IsValid(CurrentThrowableItem) && IsAvailableThrowable())
	{
		CurrentThrowableItem->Throw();
		bIsEquiping = false;
		CapacityThrowable -= 1;
		
		if(OnCurrentCapacityThrowableChangedEvent.IsBound())
		{
			OnCurrentCapacityThrowableChangedEvent.Broadcast(CapacityThrowable, MaxThrowable);
		}
		EquipItemInSlot(PreviousEquipmentSlot);
	}
}

void UCharacterEquipmentComponent::EquipItemInSlot(EEquipmentSlot Slot)
{
	if(bIsEquiping)
	{
		return;
	}
	
	UnEquipCurrentItem();
	
	CurrentEquipableItem = ItemArray[(uint32)Slot];
	CurrentEquippedWeapon = Cast<ARangeWeaponItem>(CurrentEquipableItem);
	CurrentThrowableItem = Cast<AThrowableItem>(CurrentEquipableItem);
	if(IsValid(CurrentEquipableItem))
	{
		UAnimMontage* EquipMontage = CurrentEquipableItem->GetCharacterEquipmentAnimMontage();
		if(IsValid(EquipMontage))
		{
			bIsEquiping = true;
			float DurationMontage = CacheBaseCharacter->PlayAnimMontage(EquipMontage);
			GetWorld()->GetTimerManager().SetTimer(TimerEquip, this, &UCharacterEquipmentComponent::EquipAnimationFinised, DurationMontage, false);
		}
		else
		{
			AttachCurrentItemToEquippedSocket();
		}
		
		CurrentEquipmentSlot = Slot;
		if(IsValid(CurrentEquipableItem))
		{
			CurrentEquipableItem->Equip();
		}
	}


	if(IsValid(CurrentEquippedWeapon))
	{
		OnCurrentWeaponUpdatedAmmoChangedHandle = CurrentEquippedWeapon->OnAmmoChange.AddUFunction(this, FName("OnCurrentWeaponUpdatedAmmoChanged"));
		OnWeaponReloadCompleteHandle = CurrentEquippedWeapon->OnReloadComplete.AddUFunction(this, FName("OnWeaponReloadComplete"));
		OnCurrentWeaponUpdatedAmmoChanged(CurrentEquippedWeapon->GetAmmo());
	}

	if(OnEquippedItemChangedEvent.IsBound())
	{
		OnEquippedItemChangedEvent.Broadcast(CurrentEquipableItem);
	}
	
}

void UCharacterEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("UCharacterEquipmentComponent::BeginPlay() can use only ABaseCharacter"));
	CacheBaseCharacter = StaticCast<ABaseCharacter*>(GetOwner());
	CreateLoadout();																																																																																																																					

} 

void UCharacterEquipmentComponent::EquipNextItem()
{
	uint32 CurrentEquipItemIndex = (uint32)CurrentEquipmentSlot;
	uint32 NextEquipItemIndex = NextItemArraySlotIndex(CurrentEquipItemIndex);
	while (!IsValid(ItemArray[NextEquipItemIndex]) || IgnoreSlotsWhileSwitching.Contains((EEquipmentSlot)NextEquipItemIndex))
	{
		NextEquipItemIndex = NextItemArraySlotIndex(NextEquipItemIndex);
	}
	if(CurrentEquipItemIndex != NextEquipItemIndex)
	{
		EquipItemInSlot((EEquipmentSlot)NextEquipItemIndex);
	}
}

void UCharacterEquipmentComponent::EquipPreviousItem()
{
	uint32 CurrentEquipItemIndex = (uint32)CurrentEquipmentSlot;
	uint32 PreviousEquipItemIndex = PreviousItemArraySlotIndex(CurrentEquipItemIndex);
	while (!IsValid(ItemArray[PreviousEquipItemIndex]) || IgnoreSlotsWhileSwitching.Contains((EEquipmentSlot)PreviousEquipItemIndex))
	{
		PreviousEquipItemIndex = PreviousItemArraySlotIndex(PreviousEquipItemIndex);
	}
	if(CurrentEquipItemIndex != PreviousEquipItemIndex)
	{
		EquipItemInSlot((EEquipmentSlot)PreviousEquipItemIndex);
	}
}

bool UCharacterEquipmentComponent::IsEquipping() const
{
	return bIsEquiping;
}

bool UCharacterEquipmentComponent::IsAvailableThrowable() const
{
	return  CapacityThrowable > 0;
}

int32 UCharacterEquipmentComponent::GetCapacityThrowable() const
{
	return CapacityThrowable;
}

void UCharacterEquipmentComponent::CreateLoadout()
{
	AmunitionArray.AddZeroed((uint32)EAmunitionType::MAX);

	for(const TPair<EAmunitionType, int32>& AmmoPair: MaxAmunitionAmount)
	{
		AmunitionArray[(uint32)AmmoPair.Key] = FMath::Max(AmmoPair.Value, 0);
	}

	ItemArray.AddZeroed((uint32)EEquipmentSlot::MAX);
	
	for (const TPair<EEquipmentSlot, TSubclassOf<class AEqupableItem>> ItemPair : ItemsLoadout)
	{
		if(!IsValid(ItemPair.Value))
		{
			continue;
		}
		AEqupableItem* Item = GetWorld()->SpawnActor<AEqupableItem>(ItemPair.Value);
		Item->AttachToComponent(CacheBaseCharacter->GetMesh(),FAttachmentTransformRules::KeepRelativeTransform, Item->GetUnEquippedSocketName());
		Item->SetOwner(CacheBaseCharacter.Get());
		Item->UnEquip();
		ItemArray[(uint32)ItemPair.Key] = Item;
	}
	CapacityThrowable = MaxThrowable;
	
	if(OnCurrentCapacityThrowableChangedEvent.IsBound())
	{
		OnCurrentCapacityThrowableChangedEvent.Broadcast(CapacityThrowable, MaxThrowable);
	}
}

void UCharacterEquipmentComponent::OnCurrentWeaponUpdatedAmmoChanged(int32 NewAmmo)
{
	if(OnCurrentWeaponAmmoChangedEvent.IsBound())
	{
		OnCurrentWeaponAmmoChangedEvent.Broadcast(NewAmmo, GetAvailableAmunitionForCurrentWeapon());
	}
}

int32 UCharacterEquipmentComponent::GetAvailableAmunitionForCurrentWeapon()
{
	check(GetCurrentRangeWeapon());
	
	return AmunitionArray[(uint32)GetCurrentRangeWeapon()->GetAmunitionType()];
	
}

void UCharacterEquipmentComponent::OnWeaponReloadComplete()
{
	ReloadAmmoInCurrentWeapon();
}

void UCharacterEquipmentComponent::ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo, bool bCheckIsFull)
{
	int32 AvailableAmunition = GetAvailableAmunitionForCurrentWeapon();
	int32 CurrentAmmo = CurrentEquippedWeapon->GetAmmo();
	int32 AmmoToReload = CurrentEquippedWeapon->GetMaxAmmo() - CurrentAmmo;
	int32 ReloadedAmmo = FMath::Min(AvailableAmunition, AmmoToReload);

	if(NumberOfAmmo > 0)
	{
		ReloadedAmmo = FMath::Min(ReloadedAmmo, NumberOfAmmo);
	}
	
	AmunitionArray[(uint32)GetCurrentRangeWeapon()->GetAmunitionType()] -= ReloadedAmmo;
	CurrentEquippedWeapon->SetAmmo(ReloadedAmmo + CurrentAmmo);

	if(bCheckIsFull)
	{
		AvailableAmunition = AmunitionArray[(uint32)CurrentEquippedWeapon->GetAmunitionType()];
		bool bIsFullyReloaded = CurrentEquippedWeapon->GetAmmo() == CurrentEquippedWeapon->GetMaxAmmo();
		if(AvailableAmunition == 0 || bIsFullyReloaded)
		{
			CurrentEquippedWeapon->EndReload(true);
		}
	}
}

uint32 UCharacterEquipmentComponent::NextItemArraySlotIndex(uint32 CurrentItem)
{
	uint32 NextItem = 0;
	if(CurrentItem < (uint32)ItemArray.Num() - 1)
	{
		NextItem = CurrentItem + 1;
	}

	return  NextItem;
	
}

uint32 UCharacterEquipmentComponent::PreviousItemArraySlotIndex(uint32 CurrentItem)
{
	uint32 PreviousItem = (uint32)ItemArray.Num() - 1;
	if(CurrentItem > 0)
	{
		PreviousItem = CurrentItem - 1;
	}

	return  PreviousItem;
}

void UCharacterEquipmentComponent::EquipAnimationFinised()
{
	bIsEquiping = false;
	AttachCurrentItemToEquippedSocket();
}
