// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableItem.h"
#include "Homework/Actors/Projectiles/BaseProjectile.h"
#include "Characters/BaseCharacter.h"

void AThrowableItem::Throw()
{
	ABaseCharacter* CharacterOwner = GetCharacterOwner();

	if(!IsValid(CharacterOwner))
	{
		return;
	}
	AController* Controller = CharacterOwner->GetController<AController>();
	if (!IsValid(Controller))
	{
		return;
	}

	FVector PlayerViewPoint;
	FRotator PlayerViewRotation;

	Controller->GetPlayerViewPoint(PlayerViewPoint, PlayerViewRotation);
	FTransform PlayerViewTransform(PlayerViewRotation, PlayerViewPoint);
	FVector Direction = PlayerViewRotation.RotateVector(FVector::ForwardVector);
	FVector UpVector = PlayerViewRotation.RotateVector(FVector::UpVector);

	FVector LaunchDirection = Direction + FMath::Tan(FMath::DegreesToRadians(ThrowAngle)) * UpVector;

	FVector ThrowableSocketLocation = CharacterOwner->GetMesh()->GetSocketLocation(SocketCharacterThrowable);
	FVector SocketOnViewSpace = PlayerViewTransform.InverseTransformPosition(ThrowableSocketLocation);
	
	FVector SpawnLocation = PlayerViewPoint + Direction * SocketOnViewSpace.X;
	ABaseProjectile* Projectile = GetWorld()->SpawnActor<ABaseProjectile>(ProjectileClass, SpawnLocation, FRotator::ZeroRotator);
	if(IsValid(Projectile))
	{
		Projectile->SetOwner(GetOwner());
		Projectile->SetProjectileActive(true);
		Projectile->SetActorEnableCollision(true);
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
		Projectile->OnProjectileHit.AddDynamic(this, &AThrowableItem::kek);
	}
}


void AThrowableItem::kek(ABaseProjectile* Projectile, const FHitResult& Hit, const FVector& Direction)
{
	
}

