// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNotify_RagdollEnable.h"

#include "Homework/HomeworkTypes.h"

void UAnimNotify_RagdollEnable::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	Super::Notify(MeshComp, Animation);

	MeshComp->SetCollisionProfileName(CollisionProfileRagdoll);
	MeshComp->SetSimulatePhysics(true);
}
