// Fill out your copyright notice in the Description page of Project Settings.


#include "Controller/MainPlayerController.h"

#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "EnhancedInputComponent.h"
#include "InputAction.h"
#include "InputActionValue.h"

#include "Player/MainPlayer.h"
#include "Controller/GravityGunController.h"

#include "Kismet/GameplayStatics.h"
#include "Gameplay/Goal.h"
#include "Gameplay/ScoreComponent.h"
#include "Controller/SpawnerController.h"


void AMainPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Bind to subsystem
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (!EnhancedInputSubsystem)
		return;

	EnhancedInputSubsystem->ClearAllMappings();
	EnhancedInputSubsystem->AddMappingContext(InputMapping, 0);

	// Get Enhanced Input Comp
	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);
	if (!EnhancedInputComponent)
		return;
	EnhancedInputComponent->ClearActionBindings();

	// Bind Movements
	EnhancedInputComponent->BindAction(InputActionMove, ETriggerEvent::Triggered, this, &AMainPlayerController::MovePlayer);
	EnhancedInputComponent->BindAction(InputActionLook, ETriggerEvent::Triggered, this, &AMainPlayerController::Look);
	EnhancedInputComponent->BindAction(InputActionJump, ETriggerEvent::Triggered, this, &AMainPlayerController::Jump);

	// Exercice 3
	EnhancedInputComponent->BindAction(InputActionScore, ETriggerEvent::Triggered, this, &AMainPlayerController::OnDisplayScoreInputPressed);
}

void AMainPlayerController::SetPawn(APawn* InPawn)
{
	Super::SetPawn(InPawn);

	// We want to bind only once to the inputs
	if (Character)
		return;

	// Get a ref to our Character
	Character = Cast<AMainPlayer>(InPawn);
	if (!Character)
		return;

	GravityGunController = GetComponentByClass<UGravityGunController>();
	if (GravityGunController.IsValid())
	{
		GravityGunController->SetupInputComponentGravityGun(InputComponent, Character);
	}

	// Exercice 4
	ScoreComponent = Character->GetComponentByClass<UScoreComponent>();

	// Noted Exercice 
	SpawnerController = GetComponentByClass<USpawnerController>();
	if (SpawnerController.IsValid())
	{
		SpawnerController->SetupInputPlayerController(InputComponent, Character);
	}
}

void AMainPlayerController::MovePlayer(const FInputActionValue& Value)
{
	if (!Character)
		return;

	// Get movement value
	const FVector2D MoveValue = Value.Get<FVector2D>();

	// Check if the player wants to move in the forward direction
	if (MoveValue.Y != 0.f)
	{
		Character->AddMovementInput(Character->GetActorForwardVector(), MoveValue.Y);
	}

	// Check if the player wants to move in the right direction
	if (MoveValue.X != 0.f)
	{
		Character->AddMovementInput(Character->GetActorRightVector(), MoveValue.X);
	}
}

void AMainPlayerController::Look(const FInputActionValue& Value)
{
	if (!Character)
		return;

	// Get movement value
	const FVector2D MoveValue = Value.Get<FVector2D>();

	if (MoveValue.X != 0.f)
	{
		Character->AddControllerYawInput(MoveValue.X);
	}

	if (MoveValue.Y != 0.f)
	{
		Character->AddControllerPitchInput(-MoveValue.Y);
	}
}

void AMainPlayerController::Jump()
{
	if (!Character)
		return;

	Character->Jump();
}

void AMainPlayerController::AddPitchInput(float Value)
{
	float MultipliedValue = Value * MouseSensitivityY;
	Super::AddPitchInput(MultipliedValue);
}

void AMainPlayerController::AddYawInput(float Value)
{
	float MultipliedValue = Value * MouseSensitivityX;
	Super::AddYawInput(MultipliedValue);
}

void AMainPlayerController::OnDisplayScoreInputPressed()
{
	if (!ScoreComponent.IsValid())
		return;

	ScoreComponent->OnDisplayScoreInputPressed();
}
