// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseProjectile.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Net/UnrealNetwork.h"

// Sets default values
ABaseProjectile::ABaseProjectile()
{
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));                                                                                           
	CollisionComponent->SetSphereRadius(5.f);
	CollisionComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	CollisionComponent->SetCollisionResponseToAllChannels(ECR_Block);
	SetRootComponent(CollisionComponent);

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MovementComponent"));
	ProjectileMovementComponent->InitialSpeed = 2000.f;
	ProjectileMovementComponent->bAutoActivate = false;
 
}

 

void ABaseProjectile::LaunchProjectile(FVector Direction)
{
	ProjectileMovementComponent->Velocity = Direction * ProjectileMovementComponent->InitialSpeed;
	CollisionComponent->IgnoreActorWhenMoving(GetOwner(), true);
	
	OnProjectileLaunched();
}

void ABaseProjectile::SetProjectileActive_Implementation(bool bIsProjectileActive)
{
	ProjectileMovementComponent->SetActive(bIsProjectileActive);
}

void ABaseProjectile::ProjectileDisable(FVector Loctation, FRotator Rotation, float TimerDisable)
{
	 SetProjectileActive(false);
	 SetActorEnableCollision(false);
	
	 OnProjectileHit.RemoveAll(this);
 
	GetWorld()->GetTimerManager().SetTimer(
		TimeDetonation,
		[&]()
		{
			SetActorLocation(Loctation);
			SetActorRotation(Rotation);
 		},			
		TimerDisable,
		false); 
}

float ABaseProjectile::GetTimerDetonation()
{
	return 0.f;
}
 
void ABaseProjectile::OnProjectileLaunched()
{
	
}

void ABaseProjectile::BeginPlay()
{
	Super::BeginPlay();
	CollisionComponent->OnComponentHit.AddDynamic(this, &ABaseProjectile::OnCollisionHit);
}

void ABaseProjectile::OnCollisionHit(UPrimitiveComponent* HitComponent,
	AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if(OnProjectileHit.IsBound())
	{
		OnProjectileHit.Broadcast(this, Hit, ProjectileMovementComponent->Velocity.GetSafeNormal());
	}
}
 
 



