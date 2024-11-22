// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/PickUpComponent.h"


UPickUpComponent::UPickUpComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPickUpComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

void UPickUpComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearDestructionTimer();

	// We NEED to call the endplay after or the clear timer will be called on an empty object
	Super::EndPlay(EndPlayReason);
}

void UPickUpComponent::ClearDestructionTimer()
{
	// Clear timer
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(PickUpDestructionTimerHandle);
}

void UPickUpComponent::StartPickUpDestructionTimer()
{
	// Make sure that we need to destroy it
	if (PickUpStruct.PickUpType == EPickUpType::Normal)
		return;

	// Get time
	const float TimeToDestroy = PickUpStruct.DestructionTimer;

	// Get the timer manager
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(PickUpDestructionTimerHandle);
	TimerManager.SetTimer(PickUpDestructionTimerHandle, this, &UPickUpComponent::DestroyPickUp, TimeToDestroy, false);
}

void UPickUpComponent::DestroyPickUp()
{
	ClearDestructionTimer();

	// Broadcast destruction event
	OnPickUpDestroyed.Broadcast();
	
	// Noted Exercice
	OnPickUpDestroyedWithType.Broadcast(PickUpStruct.PickUpType);

	// Destroy the pick up
	GetOwner()->Destroy();
}

EPickUpType UPickUpComponent::GetPickUpType() const
{
	return PickUpStruct.PickUpType;
}

void UPickUpComponent::OnComponentCreated()
{
	Super::OnComponentCreated();
	AActor* Owner = GetOwner();
	if (Owner && !Owner->Tags.Contains(PickUpActorTag))
	{
		Owner->Tags.Add(PickUpActorTag);
	}
}

void UPickUpComponent::OnComponentDestroyed(bool bDestroyingHierarchy)
{
#if WITH_EDITOR
	// We only want to do this if we're using UE, we won't need to do that for the executable
	if (GetWorld() && GetWorld()->IsPlayInEditor())
	{
		// Remove the tag
		AActor* Owner = GetOwner();
		if (!Owner->Tags.Contains(PickUpActorTag))
		{
			Owner->Tags.Remove(PickUpActorTag);
		}
	}
#endif

	Super::OnComponentDestroyed(bDestroyingHierarchy);
}
