// Fill out your copyright notice in the Description page of Project Settings.


#include "Zipline.h"

#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Homework/HomeworkTypes.h"
#include "Components/SceneComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Math/UnrealMathUtility.h"
#include "Homework/Characters/BaseCharacter.h"

AZipline::AZipline()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootZipline");

	FirstPillar = CreateDefaultSubobject<UStaticMeshComponent>("FirstPillar");
	FirstPillar->SetupAttachment(RootComponent);

	SecondPillar = CreateDefaultSubobject<UStaticMeshComponent>("SecondPillar");
	SecondPillar->SetupAttachment(RootComponent);

	Cable = CreateDefaultSubobject<UStaticMeshComponent>("Cable");
	Cable->SetupAttachment(RootComponent);

	InteractionVolume = CreateDefaultSubobject<UCapsuleComponent>("ZiplineInteract");
	InteractionVolume->SetupAttachment(RootComponent);
	InteractionVolume->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	InteractionVolume->SetGenerateOverlapEvents(true);
}

UCapsuleComponent* AZipline::GetZiplineInteractionCapsule() const
{
	return StaticCast<UCapsuleComponent*>(InteractionVolume);
}

void AZipline::OnConstruction(const FTransform& Transform)
{
	FirstPillar->SetRelativeLocation(FirstPillarLocation);
	SecondPillar->SetRelativeLocation(SecondPillarLocation);
	
	const FVector FirstPointForInstallationCable = FirstPillar->GetRelativeLocation() + FirstPillar->GetUpVector() * (FindPillarHalfHight(FirstPillar) + FirstPillarOffset);
	const FVector SecondPointForInstallationCable = SecondPillar->GetRelativeLocation() + SecondPillar->GetUpVector() * (FindPillarHalfHight(SecondPillar) + SecondPillarOffset);
	CenterBetweenPillars = (SecondPointForInstallationCable + FirstPointForInstallationCable ) / 2;
	Cable->SetRelativeLocation(CenterBetweenPillars);
	MaxDistance = (GetLowPillar()->GetComponentLocation() - GetHighPillar()->GetComponentLocation()).Size();
	const UStaticMesh* CableMesh = Cable->GetStaticMesh();
	if (IsValid(CableMesh))
	{
		const float MeshLength = CableMesh->GetBoundingBox().GetSize().X;
		if (!FMath::IsNearlyZero(MeshLength))
		{
			const float LengthCable = MaxDistance / MeshLength;
			Cable->SetRelativeScale3D(FVector(LengthCable, 1,1));
		}
	}
	
	Cable->SetWorldRotation(GetDirectionCable());
	
	if(IsValid(GetZiplineInteractionCapsule()))
	{
		if(!FMath::IsNearlyEqual(GetZiplineInteractionCapsule()->GetUnscaledCapsuleHalfHeight(), MaxDistance))
		{
			GetZiplineInteractionCapsule()->SetCapsuleHalfHeight(MaxDistance * 0.5f);
			GetZiplineInteractionCapsule()->SetRelativeLocation(CenterBetweenPillars);
			
			GetZiplineInteractionCapsule()->SetWorldRotation(GetDirectionCable());
			GetZiplineInteractionCapsule()->AddRelativeRotation(FRotator(90.f, 0.f, 0.f));
		}
	}
}

float AZipline::FindPillarHalfHight(const UStaticMeshComponent* Pillar)
{
	const UStaticMesh* PillarMesh = Pillar->GetStaticMesh();
	
	if(IsValid(PillarMesh))
	{	
		const float	PillarHalfHeight = PillarMesh->GetBoundingBox().GetSize().Z * 0.5f;
		return  PillarHalfHeight;
	}
	return 0.f;
}

void AZipline::BeginPlay()
{
	Super::BeginPlay();
	CableDirection = GetDirectionCable();
}

float AZipline::GetCableLength() const
{
	return MaxDistance;
}

const UStaticMeshComponent* AZipline::GetCable() const
{
	return Cable;
}

const FRotator AZipline::GetDirectionCable() const
{
	if(FMath::IsNearlyEqual(SecondPillar->GetComponentLocation().Z, FirstPillar->GetComponentLocation().Z) )
	{
		//TODO When the pillars have the same Z
		return FRotator::ZeroRotator;
	}
	else if(SecondPillar->GetComponentLocation().Z < FirstPillar->GetComponentLocation().Z)
	{
		return (SecondPillar->GetComponentLocation() - FirstPillar->GetComponentLocation()).ToOrientationRotator(); 
	}
	else
	{
		return (FirstPillar->GetComponentLocation() - SecondPillar->GetComponentLocation()).ToOrientationRotator();
	}
}

FVector AZipline::GetFirstPillarLocation() const
{
	return FirstPillar->GetComponentLocation();
}

FVector AZipline::GetSecondPillarLocation() const
{
	return SecondPillar->GetComponentLocation();
}

const UStaticMeshComponent* AZipline::GetHighPillar() const 
{
	return FirstPillar->GetComponentLocation().Z > SecondPillar->GetComponentLocation().Z ? FirstPillar : SecondPillar;
}

const UStaticMeshComponent* AZipline::GetLowPillar() const
{
	return FirstPillar->GetComponentLocation().Z < SecondPillar->GetComponentLocation().Z ? FirstPillar : SecondPillar;
}

const FVector AZipline::GetCenterBetweenPillars() const
{
	return CenterBetweenPillars;
}

const FRotator AZipline::GetDirectionCableStatic() const
{
	return CableDirection;
}

