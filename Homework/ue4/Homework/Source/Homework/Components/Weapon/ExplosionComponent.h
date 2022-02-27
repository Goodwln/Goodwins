// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "ExplosionComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExplosion);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class HOMEWORK_API UExplosionComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void Explode(AController* Controller);
	
	UPROPERTY(BlueprintAssignable)
	FOnExplosion OnExplosion;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage", meta = (UIMin = 1.f, ClampMin = 1.f))
	float MaxDamage = 75.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage", meta = (UIMin = 1.f, ClampMin = 1.f))
	float MinDamage = 30.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage", meta = (UIMin = 1.f, ClampMin = 1.f))
	float DamageFalloff = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Damage")
	TSubclassOf<class UDamageType> DamageTypeClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Radius", meta = (UIMin = 0.1f, ClampMin = 0.1f))
	float InnerRadius = 300.f;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Radius", meta = (UIMin = 1.f, ClampMin = 1.f))
	float OuterRadius= 500.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Explosion | Radius")
	UParticleSystem* ExplosionVFX;
public:	

		
};
