// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseProjectile.h"

#include "ExplosiveProjectile.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ETimerDetonationProperty : uint8
{
	None,
	TimerActivateAtLaunch,
	TimerActivationAtHit,
	MAX UMETA(Hidden)
};

class UExplosionComponent;
UCLASS()
class HOMEWORK_API AExplosiveProjectile : public ABaseProjectile
{
	GENERATED_BODY()

public:
	AExplosiveProjectile();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UExplosionComponent* ExplosionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	float DetonationTime = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Explosion")
	ETimerDetonationProperty TimerDetonationProperty = ETimerDetonationProperty::TimerActivateAtLaunch;
	
	virtual void OnProjectileLaunched() override;
	virtual void OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	
	AController* GetController();
private:
	void OnDetonationTimerElapse();
	FTimerHandle DetonationTimer;
	
};
