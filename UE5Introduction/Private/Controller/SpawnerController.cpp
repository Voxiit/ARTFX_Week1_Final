// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/SpawnerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"

#include "Player/MainPlayer.h"
#include "Gameplay/SpawnerComponent.h"
#include "Gameplay/GravityGunComponent.h"

USpawnerController::USpawnerController()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USpawnerController::SetupInputPlayerController(TObjectPtr<class UInputComponent> InputComponent, AMainPlayer* InCharacter)
{
	if (!InCharacter)
		return;

	// Get references
	Character = InCharacter;
	SpawnerComponent = Character->FindComponentByClass<USpawnerComponent>();
	GravityGunComponent = Character->FindComponentByClass<UGravityGunComponent>();

	// Get Enhanced Input Comp
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
		return;

	// Bind actions
	EnhancedInputComponent->BindAction(InputActionSpawnPickUp, ETriggerEvent::Triggered, this, &USpawnerController::OnSpawnPickUpInputPressed);
	EnhancedInputComponent->BindAction(InputActionPickUpInfo, ETriggerEvent::Triggered, this, &USpawnerController::OnPickUpInfoInputPressed);
	EnhancedInputComponent->BindAction(InputActionDestroyPickUp, ETriggerEvent::Triggered, this, &USpawnerController::OnDestroyPickUpPressed);
}

void USpawnerController::OnSpawnPickUpInputPressed(const FInputActionValue& Value)
{
	if (!SpawnerComponent.IsValid())
		return;

	// Get movement value
	const FVector2D MoveValue = Value.Get<FVector2D>();

	// X positive = Normal Pick Up
	if (MoveValue.X > 0)
	{
		SpawnerComponent->OnSpawnPickUpInputPressed(EPickUpType::Normal);
		return;
	}

	// X negative = Destroy After Throw
	if (MoveValue.X < 0)
	{
		SpawnerComponent->OnSpawnPickUpInputPressed(EPickUpType::DestroyAfterThrow);
		return;
	}

	// Y = Destroy After Pick Up
	if (MoveValue.Y >= 0)
	{
		SpawnerComponent->OnSpawnPickUpInputPressed(EPickUpType::DestroyAfterPickUp);
		return;
	}
}

void USpawnerController::OnPickUpInfoInputPressed()
{
	if (!SpawnerComponent.IsValid())
		return;

	SpawnerComponent->OnPickUpInfoInputPressed();
}

void USpawnerController::OnDestroyPickUpPressed()
{
	if (!GravityGunComponent.IsValid())
		return;

	if (GravityGunComponent->HasPickUpInHands())
		GravityGunComponent->DestroyPickUpInHands();
}

