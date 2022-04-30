// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkTypes.h"
#include "Actors/Equipment/Throwables/ThrowableItem.h"
#include "Actors/Equipment/Weapons/MeleeWeaponItem.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Components/ActorComponent.h"
#include "CharacterEquipmentComponent.generated.h"

class AEqupableItem;
class ARangeWeaponItem;
class AThrowableItem;
class MeleeWeaponItem;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChangedEvent, int32, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentCapacityThrowableChangedEvent, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChangedEvent, const AEqupableItem*)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UCharacterEquipmentComponent();
 	
	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeapon() const;

	AMeleeWeaponItem* GetCurrentMeleeWeaponItem() const;
	
	void ReloadCurrentWeapon();

	void ReloadAmmoInCurrentWeapon(int32 NumberOfAmmo = 0, bool bCheckIsFull = false);

	void UnEquipCurrentItem();

	void AttachCurrentItemToEquippedSocket();

	void LaunchThrowCurrenThrowable();
  	
	FOnCurrentWeaponAmmoChangedEvent OnCurrentWeaponAmmoChangedEvent;
	FOnCurrentCapacityThrowableChangedEvent OnCurrentCapacityThrowableChangedEvent;
	FOnEquippedItemChangedEvent OnEquippedItemChangedEvent;
	
	void EquipNextItem();
	void EquipPreviousItem();
	void EquipItemInSlot(EEquipmentSlot Slot);
	bool IsEquipping() const;

	bool IsAvailableThrowable() const;
	int32 GetCapacityThrowable() const;

	void ClearEquip();
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSubclassOf<ARangeWeaponItem> SideArmClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlot, TSubclassOf<AEqupableItem>> ItemsLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSet<EEquipmentSlot> IgnoreSlotsWhileSwitching;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	EEquipmentSlot AutoEquipmentSlot = EEquipmentSlot::None;

private:
  
	int32 CapacityThrowable = 0;

	UPROPERTY( )
	TArray<int32> AmunitionArray;

	UPROPERTY( )
	TArray<AEqupableItem*> ItemArray;
 
	void CreateLoadout();

	void AutoEquip();

	TWeakObjectPtr<ABaseCharacter> CacheBaseCharacter;

	UFUNCTION()
	void OnCurrentWeaponUpdatedAmmoChanged(int32 NewAmmo);

	int32 GetAvailableAmunitionForCurrentWeapon();

	UFUNCTION()
	void OnWeaponReloadComplete();

	UPROPERTY( )
	EEquipmentSlot PreviousEquipmentSlot;

	UPROPERTY( )
	EEquipmentSlot CurrentEquipmentSlot;
 
	UPROPERTY( )
	AEqupableItem* CurrentEquipableItem;
	UPROPERTY( )
	ARangeWeaponItem* CurrentEquippedWeapon;
	UPROPERTY( )
	AThrowableItem* CurrentThrowableItem;
	UPROPERTY( )
	AMeleeWeaponItem* CurrentMeleeWeaponItem;
	
	FDelegateHandle OnCurrentWeaponUpdatedAmmoChangedHandle;
	FDelegateHandle OnWeaponReloadCompleteHandle;
	

	uint32 NextItemArraySlotIndex(uint32 CurrentItem);
	uint32 PreviousItemArraySlotIndex(uint32 CurrentItem);

	bool bIsEquiping = false;

	FTimerHandle TimerEquip;

	void EquipAnimationFinised();
};
