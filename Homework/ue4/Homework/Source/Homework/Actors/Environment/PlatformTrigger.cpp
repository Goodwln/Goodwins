// Fill out your copyright notice in the Description page of Project Settings.


#include "PlatformTrigger.h"
#include "HomeworkTypes.h"
#include "Characters/BaseCharacter.h"
#include "Components/BoxComponent.h"
#include "Net/UnrealNetwork.h"

APlatformTrigger::APlatformTrigger()
{
 
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	TriggerBox->SetCollisionProfileName(CollisionProfilePawnInteractionVolume);
	SetRootComponent(TriggerBox);
}

void APlatformTrigger::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{

	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

void APlatformTrigger::BeginPlay()
{
	Super::BeginPlay();
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &APlatformTrigger::OnTriggerOverlapEnd);
}

void APlatformTrigger::SetIsActivated(bool bIsActivated_In)
{
 
	if (OnTriggerActivated.IsBound())
	{
		OnTriggerActivated.Broadcast(bIsActivated_In);
	}
}


void APlatformTrigger::OnTriggerOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
	if (!IsValid(BaseCharacter))
	{
		return;
	}
	if(BaseCharacter->IsLocallyControlled() )
	{
		OverlappedPawns.AddUnique(BaseCharacter);
		
		if (!bIsActivated && OverlappedPawns.Num() > 0)
		{
			bIsActivated = true;
			SetIsActivated(bIsActivated);
			
		}
	}
}

void APlatformTrigger::OnTriggerOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	ABaseCharacter* BaseCharacter = Cast<ABaseCharacter>(OtherActor);
	if (!IsValid(BaseCharacter))
	{
		return;
	}
	if(BaseCharacter->IsLocallyControlled() )
	{
		OverlappedPawns.RemoveSingleSwap(BaseCharacter);

		if (bIsActivated && OverlappedPawns.Num() == 0)
		{
			bIsActivated = false;
			SetIsActivated(bIsActivated);
		}
	}
}
