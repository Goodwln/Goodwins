//Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemAttributeComponent.generated.h"

class ATurret;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnItemDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FiveParams(FOnItemTakeDamageSignature, AActor*, DamagedActor, float, Damage, const class UDamageType*, DamageType, class AController*, InstigatedBy, AActor*, DamageCauser);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK_API UItemAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	
	FOnItemTakeDamageSignature OnItemTakeDamageEvent;
	
	UPROPERTY(BlueprintCallable)
	FOnItemDeathSignature OnItemDeathEvent;

	bool IsDeath() const;
protected:
	UItemAttributeComponent();
	
	virtual void BeginPlay() override;
	
	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	float MaxHealth = 50.f;

	
private:
	float CurrentHealth = 0.f;

	void Killed(AController* KillerController);
	
};
