// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HomeworkTypes.h"
#include "Net/UnrealNetwork.h"
#include "CharacterAttributeComponent.generated.h"

USTRUCT(BlueprintType)
struct FDefaultAttributeProperty
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FLinearColor Color = FLinearColor::White;
};


DECLARE_MULTICAST_DELEGATE(FOnDeathEventSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathBlueprintImplamentationEventSignature);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FChangeStaminaEventSignature, float, float, FDefaultAttributeProperty);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChangeOxygenEventSignature, float, float, FDefaultAttributeProperty);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnChangeHealthEventSignature, float, float, FDefaultAttributeProperty);
DECLARE_MULTICAST_DELEGATE_FiveParams(FOnTakeAnyDamageEventSignature, AActor*, float, const class UDamageType*,
					class AController*, AActor*);

class ABaseCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK_API UCharacterAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCharacterAttributeComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
  
	FOnDeathEventSignature OnDeathEvent;

	UPROPERTY(BlueprintCallable)
	FOnDeathBlueprintImplamentationEventSignature OnDeathBlueprintImplamentationEvent;

	FChangeStaminaEventSignature OnChangeStaminaEvent;
	FOnChangeOxygenEventSignature OnChangeOxygenEvent;
	FOnChangeHealthEventSignature OnChangeHealthEvent;
	FOnTakeAnyDamageEventSignature OnTakeAnyDamageEvent;
	
	bool IsAlive() { return CurrentHealth > 0.f; }

	void SetIsActiveStamina(bool IsActive) { bIsActiveStamina = IsActive; }
	bool GetIsActiveStamina() const  { return bIsActiveStamina; }

	void SetIsActiveOxygen(bool IsActive) { bIsActiveOxygen = IsActive; }
	bool GetIsActiveOxygen() const { return bIsActiveOxygen; }

	float GetHealthPercent() const;
	float GetOxygenPercent() const;
	float GetStaminaPercent() const;
	
	UFUNCTION()
	void TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType,
					class AController* InstigatedBy, AActor* DamageCauser);
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health", meta = (UIMin = 0.f))
	float MaxHealth = 0.f;
	
	void UpdateStaminaValue(float DeltaTime);
	void UpdateOxygenValue(float DeltaTime);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina", meta = (UIMin = 0.f))
	float MaxStamina = 0.f; // max value player stamina
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina", meta = (UIMin = 0.f))
	float StaminaRestoreVelocity = 0.f; // stamina speed restore
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stamina", meta = (UIMin = 0.f))
	float SprintStaminaConsumptionVelocity = 0.f; //speed consumption player stamina
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen", meta = (UIMin = 0.f))
	float MaxOxygen = 50.0f; //max velocity oxyge
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen", meta = (UIMin = 0.f))
	float OxygenRestoreVelocity = 15.0f; //speed restore oxygen per sec
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Oxygen", meta = (UIMin = 0.f))
	float SwimOxygenConsumptionVelocity = 2.0f; //speed consumption oxygen per sec

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "DefaultAttributeProperty")
	TMap <ECurrentAttribute, FDefaultAttributeProperty> AttributeProperty;
	
#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	void DebugDrawAttribute();
#endif
	
private:
	TWeakObjectPtr<ABaseCharacter> CacheBaseCharacter;

	UPROPERTY( )
	float CurrentHealth = 0.f;
 

	void OnHealthChanged(AController* KillerController);
	
	float CurrentStamina = 0.f;
	float CurrentOxygen = 0.f;
	bool bIsActiveStamina = false;
	bool bIsActiveOxygen = false;

	void Killed(AController* Controller);
}; 

