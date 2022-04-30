// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/Equipment/EqupableItem.h"
#include "ThrowableItem.generated.h"

/**
 * 
 */
UCLASS(Blueprintable)
class HOMEWORK_API AThrowableItem : public AEqupableItem
{
	GENERATED_BODY()

public:
	void Throw();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables")
	TSubclassOf<class ABaseProjectile> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Throwables", meta = (UIMin = -90, UIMax = 90, ClampMin = -90, ClampMax = 90))
	float ThrowAngle = 30.f;

private:
	UFUNCTION()
	void kek(ABaseProjectile* Projectile, const FHitResult& Hit, const FVector& Direction);
};
