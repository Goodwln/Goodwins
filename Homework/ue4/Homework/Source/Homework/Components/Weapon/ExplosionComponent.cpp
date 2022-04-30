// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosionComponent.h"

#include "Actors/Projectiles/BaseProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystem.h"


UExplosionComponent::UExplosionComponent()
{
	
}

void UExplosionComponent::BeginPlay()
{
	Super::BeginPlay();
	ExplosionLocation = GetComponentLocation();
}

void UExplosionComponent::Explode(AController* Controller)
{
	TArray<AActor*> IgnoreActors;
	IgnoreActors.Add(GetOwner());
	
	UGameplayStatics::ApplyRadialDamageWithFalloff(
		GetWorld(),
		MaxDamage,
		MinDamage,
		ExplosionLocation,
		InnerRadius,
		OuterRadius,
		DamageFalloff,
		DamageTypeClass, 
		IgnoreActors,   
		GetOwner(),
		Controller,
		ECC_Visibility
	);
	
	if(IsValid(ExplosionVFX))
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ExplosionVFX, ExplosionLocation);
	}
	
	if(OnExplosion.IsBound())
	{
		OnExplosion.Broadcast();
	}
}

void UExplosionComponent::SetExplosionLocation(FVector NewExplosionLocation)
{
	ExplosionLocation = NewExplosionLocation;
}
 
