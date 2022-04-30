// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "MeleeHitRegistrator.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMeleeHitRegistred, const FHitResult&,Hit, const FVector&, Location);
UCLASS(meta = (BlueprintSpawnableComponent))
class HOMEWORK_API UMeleeHitRegistrator : public USphereComponent
{
	GENERATED_BODY()

public:
	UMeleeHitRegistrator();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	void ProcessHitRegistration();

	void SetIsHitRegistrationEnable(bool bIsEnabled_In);

	FOnMeleeHitRegistred OnMeleeHitRegistred;
protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Melee hit registration")
	bool bIsHitRegistrationEnabled = false;

private:
	FVector PreviousComponentLocation = FVector::ZeroVector;
	
};
