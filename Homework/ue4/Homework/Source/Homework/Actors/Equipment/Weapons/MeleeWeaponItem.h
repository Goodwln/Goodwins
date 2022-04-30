// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EqupableItem.h"
#include "MeleeWeaponItem.generated.h"

USTRUCT(BlueprintType)
struct FMeleeAttackDescription
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Melee attack")
	TSubclassOf<class UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Melee attack", meta = (ClampMin = 0.f, UIMin = 0.f))
	float DamageAmount = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "Melee attack")
	class UAnimMontage* AttackMontage;

	
};

class UMeleeHitRegistrator;
UCLASS(Blueprintable)
class HOMEWORK_API AMeleeWeaponItem : public AEqupableItem
{
	GENERATED_BODY()
public:
	AMeleeWeaponItem();

	void StartAttack(EMeleeAttackType AttackType);

	void SetIsHitRegistrationEnabled(bool bIsRegistrationEnabled);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly,Category = "All Melee attacks")
	TMap<EMeleeAttackType, FMeleeAttackDescription> Attacks;

	virtual void BeginPlay() override;
private:

	UFUNCTION()
	void ProcessHit(const FHitResult& HitResult, const FVector& Direction);
	
	TArray<UMeleeHitRegistrator*> HitRegistrators;
	TSet<AActor*> HitActors;
	
	void OnAttackTimerElapsed();
	FTimerHandle AttackTimer;
	FMeleeAttackDescription* CurrentAttack = nullptr;
};
