// Fill out your copyright notice in the Description page of Project Settings.


#include "ExplosiveProjectile.h"

 
#include "Homework/Components/Weapon/ExplosionComponent.h"

AExplosiveProjectile::AExplosiveProjectile()
{
	ExplosionComponent = CreateDefaultSubobject<UExplosionComponent>(TEXT("ExplosionComponent"));
	ExplosionComponent->SetupAttachment(GetRootComponent());
}

float AExplosiveProjectile::GetTimerDetonation()
{
	return DetonationTime;
}

void AExplosiveProjectile::OnProjectileLaunched()
{                              
	Super::OnProjectileLaunched();
	if(TimerDetonationProperty == ETimerDetonationProperty::TimerActivateAtLaunch)
	{
		GetWorld()->GetTimerManager().SetTimer(DetonationTimer, this, &AExplosiveProjectile::OnDetonationTimerElapse, DetonationTime, false);
	}
}

void AExplosiveProjectile::OnCollisionHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	Super::OnCollisionHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
 
	if(TimerDetonationProperty == ETimerDetonationProperty::TimerActivationAtHit)
	{
		GetWorld()->GetTimerManager().SetTimer(DetonationTimer, this, &AExplosiveProjectile::OnDetonationTimerElapse, DetonationTime, false);

	}
}
                                   
AController* AExplosiveProjectile::GetController()
{
	APawn* PawnOwner = Cast<APawn>(GetOwner());
	return IsValid(PawnOwner) ? PawnOwner->GetController() : nullptr;
}

void AExplosiveProjectile::OnDetonationTimerElapse()
{
	 ExplosionComponent->SetExplosionLocation(GetActorLocation());
	 ExplosionComponent->Explode(GetController());
	
}
 
