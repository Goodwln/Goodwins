// Fill out your copyright notice in the Description page of Project Settings.


#include "AICharacter.h"
#include "Components/WidgetComponent.h"
#include "Perception/AISense_Damage.h"
#include "UI/Widget/HealthBarWidget.h"

AAICharacter::AAICharacter(const FObjectInitializer& ObjectInitializer)
	:Super(ObjectInitializer)
{
	AutoPossessAI = EAutoPossessAI::Disabled;

	HealthWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("HealthBarWidget"));
	HealthWidgetComponent->SetupAttachment(GetRootComponent());
	HealthWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	HealthWidgetComponent->SetDrawAtDesiredSize(true);
}

UBehaviorTree* AAICharacter::GetBehaviorTree() const
{
	return BehaviorTree;
}

void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterAttributeComponent()->OnTakeAnyDamageEvent.AddUObject(this, &AAICharacter::TakeAnyDamage);
 
}

void AAICharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	UpdateHealthWidgetVisibility();
}


void AAICharacter::TakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
                                 AController* InstigatedBy, AActor* DamageCauser)
{
	UAISense_Damage::ReportDamageEvent(
		GetWorld(),
		DamagedActor,
		DamageCauser,
		Damage,
		DamageCauser->GetActorLocation(),
		DamagedActor->GetActorLocation());
}

void AAICharacter::OnChangeHealth(float Current, float Max, FDefaultAttributeProperty Attribute)
{
	Super::OnChangeHealth(Current, Max, Attribute);

	UHealthBarWidget* HealthBarWidget = Cast<UHealthBarWidget>(HealthWidgetComponent->GetUserWidgetObject());
	if(IsValid(HealthBarWidget))
	{
		HealthBarWidget->SetHealthPercent(Current / Max);
	}
}

void AAICharacter::UpdateHealthWidgetVisibility()
{
	if(!IsValid(GetWorld()) ||										//
	   !IsValid(GetWorld()->GetFirstPlayerController()) ||			//
	   !IsValid(GetWorld()->GetFirstPlayerController()->GetPawn()))	
	{
		return;
	}
	
	const FVector PlayerLocation = GetWorld()->GetFirstPlayerController()->GetPawn()->GetActorLocation();
	const float Distance = FVector::Distance(PlayerLocation, GetActorLocation());
	HealthWidgetComponent->SetVisibility(Distance < HealthVisibilityDistance, true);;
}
