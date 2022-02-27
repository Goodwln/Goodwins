// Fill out your copyright notice in the Description page of Project Settings.


#include "ThrowableItem.h"
#include "Homework/Actors/Projectiles/BaseProjectile.h"
#include "Characters/BaseCharacter.h"

void AThrowableItem::Throw()
{
	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("ARangeWeaponItem::Fire() only character can be an owner of range weapon"));
	ABaseCharacter* CharacterOwner = StaticCast<ABaseCharacter*>(GetOwner());

	APlayerController* Controller = CharacterOwner->GetController<APlayerController>();
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
		Projectile->LaunchProjectile(LaunchDirection.GetSafeNormal());
		
	}                                                                                                                                                                                                                                                       
}

