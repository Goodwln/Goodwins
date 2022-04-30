//Fill out your copyright notice in the Description page of Project Settings.


#include "ItemAttributeComponent.h"

#include "GameModeLearning.h"

// Sets default values for this component's properties
UItemAttributeComponent::UItemAttributeComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UItemAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	GetOwner()->OnTakeAnyDamage.AddDynamic(this, &UItemAttributeComponent::TakeDamage);
	CurrentHealth = MaxHealth;
	
}

void UItemAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void UItemAttributeComponent::TakeDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
	if(IsDeath())
	{
		return;
	}

	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);

	if(OnItemTakeDamageEvent.IsBound())
	{
		OnItemTakeDamageEvent.Broadcast(DamagedActor, Damage, DamageType, InstigatedBy, DamageCauser);
	}
	
	if(IsDeath())
	{
		Killed(InstigatedBy);
		if(OnItemDeathEvent.IsBound())
		{
			OnItemDeathEvent.Broadcast();
		}
	}
}

void UItemAttributeComponent::Killed(AController* KillerController)
{
	if(!IsValid(GetWorld()))
	{
		return;
	}
	
	AGameModeLearning* GameMode = Cast<AGameModeLearning>(GetWorld()->GetAuthGameMode());
	if(!IsValid(GameMode))
	{
		return;
	}

	APawn* Player = Cast<APawn>(GetOwner());
	AController* VictimController = IsValid(Player) ? Player->GetController() : nullptr;
 

	GameMode->Killed(KillerController, VictimController);
}


bool UItemAttributeComponent::IsDeath() const
{
	return CurrentHealth <= 0.f;
}
