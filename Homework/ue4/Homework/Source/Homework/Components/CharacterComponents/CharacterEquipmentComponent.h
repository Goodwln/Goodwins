// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkTypes.h"
#include "Actors/Equipment/Throwables/ThrowableItem.h"
#include "Actors/Equipment/Weapons/RangeWeaponItem.h"
#include "Components/ActorComponent.h"
#include "CharacterEquipmentComponent.generated.h"

class AEqupableItem;
class ARangeWeaponItem;
class AThrowableItem;

typedef TArray<class AEqupableItem*, TInlineAllocator<(uint32)EEquipmentSlot::MAX>> TItemArray;
typedef TArray<int32, TInlineAllocator<(uint32)EAmunitionType::MAX>> TAmunitionArray;

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentWeaponAmmoChangedEvent, int32, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnCurrentCapacityThrowableChangedEvent, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEquippedItemChangedEvent, const AEqupableItem*)

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK_API UCharacterEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	EEquipableItemType GetCurrentEquippedItemType() const;

	ARangeWeaponItem* GetCurrentRangeWeapon() const;
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
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSubclassOf<class ARangeWeaponItem> SideArmClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EAmunitionType, int32> MaxAmunitionAmount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TMap<EEquipmentSlot, TSubclassOf<class AEqupableItem>> ItemsLoadout;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Loadout")
	TSet<EEquipmentSlot> IgnoreSlotsWhileSwitching;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Grenade")
	int32 MaxThrowable = 1;

private:

	int32 CapacityThrowable = 0;
	
	TAmunitionArray AmunitionArray;
	TItemArray ItemArray;
	
	void CreateLoadout();


	TWeakObjectPtr<class ABaseCharacter> CacheBaseCharacter;

	UFUNCTION()
	void OnCurrentWeaponUpdatedAmmoChanged(int32 NewAmmo);

	int32 GetAvailableAmunitionForCurrentWeapon();

	UFUNCTION()
	void OnWeaponReloadComplete();

	EEquipmentSlot PreviousEquipmentSlot;
	EEquipmentSlot CurrentEquipmentSlot;
	
	AEqupableItem* CurrentEquipableItem;
	ARangeWeaponItem* CurrentEquippedWeapon;
	AThrowableItem* CurrentThrowableItem;
	
	FDelegateHandle OnCurrentWeaponUpdatedAmmoChangedHandle;
	FDelegateHandle OnWeaponReloadCompleteHandle;
	

	uint32 NextItemArraySlotIndex(uint32 CurrentItem);
	uint32 PreviousItemArraySlotIndex(uint32 CurrentItem);

	bool bIsEquiping = false;

	FTimerHandle TimerEquip;

	void EquipAnimationFinised();
};
