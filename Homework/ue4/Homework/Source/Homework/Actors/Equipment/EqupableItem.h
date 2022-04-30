// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "HomeworkTypes.h"
#include "GameFramework/Actor.h"
#include "EqupableItem.generated.h"

class ABaseCharacter;
class UAnimMontage;
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnEquipmentStateChanged, bool, bIsEquipped);

UCLASS(Abstract,NotBlueprintable)
class HOMEWORK_API AEqupableItem : public AActor
{
	GENERATED_BODY()
public:
	AEqupableItem();
	
	virtual void SetOwner(AActor* NewOwner) override;
	
	EEquipableItemType GetItemType() const { return  ItemType; }

	FName GetUnEquippedSocketName() const;

	FName GetEquippedSocketName() const;

	UAnimMontage* GetCharacterEquipmentAnimMontage() const;

	virtual void Equip();
	virtual void UnEquip();

	virtual  ECurrentReticle GetCurrentReticle() const;

protected:
								
	UPROPERTY(BlueprintAssignable)
	FOnEquipmentStateChanged OnEquipmentStateChanged;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	EEquipableItemType ItemType = EEquipableItemType::None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	UAnimMontage* CharacterEquipmentAnimMontage;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName UnEquippedSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Equipable item")
	FName EquippedSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Reticle")
	ECurrentReticle CurrentReticle = ECurrentReticle::None;

	ABaseCharacter* GetCharacterOwner() const;
private:
	TWeakObjectPtr<ABaseCharacter> CachedBaseCharacter;
	
};
