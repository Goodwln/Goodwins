// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAttributeComponent.h"
#include "DrawDebugHelpers.h"
#include "Components/CapsuleComponent.h"
#include "Homework/HomeworkTypes.h"
#include "Homework/Characters/BaseCharacter.h"
#include "Homework/Subsystems/DebugSubsystem.h"
#include "Kismet/GameplayStatics.h"

// Sets default values for this component's properties
UCharacterAttributeComponent::UCharacterAttributeComponent()
{

	PrimaryComponentTick.bCanEverTick = true;

}

void UCharacterAttributeComponent::BeginPlay()
{
	Super::BeginPlay();

	checkf(GetOwner()->IsA<ABaseCharacter>(), TEXT("UCharacterAttributeComponent::UCharacterAttributeComponent() can be use only with ABaseCharacter"));
	CacheBaseCharacter = StaticCast<ABaseCharacter*>(GetOwner());

	CacheBaseCharacter->OnTakeAnyDamage.AddDynamic(this, &UCharacterAttributeComponent::TakeDamage);
	CurrentHealth = MaxHealth;

	const UEnum* AttributeList = StaticEnum<ECurrentAttribute>();
	for (int32 i = 1; i < AttributeList->NumEnums() - 1; i++)
	{
		const ECurrentAttribute EnumType = static_cast<ECurrentAttribute>(i);
		const FString EnumTypeName = UEnum::GetValueAsString(EnumType);
		const bool AttributeCheckCond = AttributeProperty.Contains(EnumType);
		checkf(AttributeCheckCond, TEXT("AttributeProperty for %s doesn't exist"), *EnumTypeName);
	}              
}

float UCharacterAttributeComponent::GetHealthPercent() const
{
	return CurrentHealth / MaxHealth;
}

float UCharacterAttributeComponent::GetOxygenPercent() const
{
	return CurrentOxygen / MaxOxygen;
}

float UCharacterAttributeComponent::GetStaminaPercent() const
{
	return CurrentStamina / MaxStamina;
}

void UCharacterAttributeComponent::TakeDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                              AController* InstigatedBy, AActor* DamageCauser)
{
	if(!IsAlive())
	{
		return;
	}
	
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
//	UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::TakeDamage %s, CurrentHealth: %.2f, Damage: %.2f, Enemy: %s"), *CacheBaseCharacter->GetName(), CurrentHealth, Damage, *DamageCauser->GetName());

	if(OnChangeHealthEvent.IsBound())
	{
		OnChangeHealthEvent.Broadcast(CurrentHealth, MaxHealth, AttributeProperty[ECurrentAttribute::Health]);
	}
	
	if(CurrentHealth <= 0.f)
	{
		//UE_LOG(LogDamage, Warning, TEXT("UCharacterAttributeComponent::TakeDamage %s Is killed by actor, Enemy: %s"), *CacheBaseCharacter->GetName(), *DamageCauser->GetName());
		
		OnDeathEvent.Broadcast();
	}
}


void UCharacterAttributeComponent::UpdateStaminaValue(float DeltaTime)
{
	const float PrevStamina = CurrentStamina;
	if(!GetIsActiveStamina())
	{
		CurrentStamina = FMath::Clamp(CurrentStamina + StaminaRestoreVelocity * DeltaTime ,0.f, MaxStamina);
	}

	if(GetIsActiveStamina())
	{
		CurrentStamina = FMath::Clamp(CurrentStamina - SprintStaminaConsumptionVelocity * DeltaTime, 0.f, MaxStamina);
	}
  
	if(PrevStamina != CurrentStamina)
	{
		OnChangeStaminaEvent.Broadcast(CurrentStamina, MaxStamina, AttributeProperty[ECurrentAttribute::Stamina]);	
	}
}

void UCharacterAttributeComponent::UpdateOxygenValue(float DeltaTime)
{
	const float PrevOxygen = CurrentOxygen;
	if(!GetIsActiveOxygen())
	{
		CurrentOxygen = FMath::Clamp(CurrentOxygen + OxygenRestoreVelocity * DeltaTime, 0.f, MaxOxygen);
	}
	
	if(GetIsActiveOxygen())
	{
		CurrentOxygen = FMath::Clamp(CurrentOxygen - SwimOxygenConsumptionVelocity * DeltaTime, 0.f, MaxOxygen);
	}

	if(PrevOxygen != CurrentOxygen)
	{
		OnChangeOxygenEvent.Broadcast(CurrentOxygen, MaxOxygen, AttributeProperty[ECurrentAttribute::Oxygen]);
	}
}

void UCharacterAttributeComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
	DebugDrawAttribute();
#endif
	
	UpdateStaminaValue(DeltaTime);
	UpdateOxygenValue(DeltaTime);
}


#if UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT
void UCharacterAttributeComponent::DebugDrawAttribute()
{
	UDebugSubsystem* DebugSubsystem = UGameplayStatics::GetGameInstance(GetWorld())->GetSubsystem<UDebugSubsystem>();
	if(!DebugSubsystem->IsCategoryEnable(DebugCategoryCharacterAttributes))
	{
		return;
	}

	const float CapsuleHalfHeight = CacheBaseCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	FVector TextLocation = CacheBaseCharacter->GetActorLocation() + (CapsuleHalfHeight + 5.f) * FVector::UpVector;
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Health: %.2f"), CurrentHealth), nullptr, FColor::Red,0.f, true);

	TextLocation = TextLocation + FVector::UpVector * -50.f;
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Stamina: %.2f"), CurrentStamina), nullptr, FColor::Green,0.f, true);

	TextLocation = TextLocation + FVector::UpVector * -20.f + FVector::RightVector * -50.f;
	DrawDebugString(GetWorld(), TextLocation, FString::Printf(TEXT("Oxygen: %.2f"), CurrentOxygen), nullptr, FColor::Blue,0.f, true);

}

#endif

