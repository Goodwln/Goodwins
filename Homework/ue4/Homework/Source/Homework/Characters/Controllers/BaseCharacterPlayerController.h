// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "GameFramework/PlayerController.h"
#include "BaseCharacterPlayerController.generated.h"

class UPlayerHUDWidget;
class ABaseCharacter;

class  URespawnComponent;
 
UCLASS()
class HOMEWORK_API ABaseCharacterPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	ABaseCharacterPlayerController();
	
	virtual void OnPossess(APawn* InPawn) override;
	URespawnComponent* GetRespawnComponent() const { return RespawnComponent; }

protected:

	virtual void SetupInputComponent() override;

	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UPlayerHUDWidget> PlayerHudWidgetClass;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	URespawnComponent* RespawnComponent;
private:
	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);
	void ChangeCrouchState();
	void Mantle();
	void Jump();

	void ClimbLadderUp(float Value);
	void InteractWithLadder();

	void InteractWithZipline();
	
	void StartSprint();
	void StopSprint();

	void SwimForward(float Value);
	void SwimRight(float Value);
	void SwimUp(float Value);

	void WallRun(float Value);
	
	void StartSlide();
	void StopSlide();
	
	void ChangeProneState();

	void FireStart();
	void FireStop();

	void StartAiming();
	void StopAiming();

	void Reload();

	void NextItem();
	void PreviousItem();

	void EquipPrimaryItem();

	void SecondaryFire();

	void PrimaryMeleeAttack();
	void SecondaryMeleeAttack();
	TWeakObjectPtr< ABaseCharacter> CachedBaseCharacter;

	UPlayerHUDWidget* PlayerHUDWidget;

	void CreateAndInitializeWidget();
};