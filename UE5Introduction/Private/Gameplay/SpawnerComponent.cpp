// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/SpawnerComponent.h"
#include "Gameplay/GravityGunComponent.h"
#include "Gameplay/PickUpComponent.h"

#include "Player/MainPlayer.h"

#include "Kismet/GameplayStatics.h"
#include "UObject/ObjectSaveContext.h"


USpawnerComponent::USpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void USpawnerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// Get pointers
	Character = Cast<AMainPlayer>(GetOwner());
	PlayerCameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	if (Character.IsValid())
	{
		GravityGunComponent = Character->FindComponentByClass<UGravityGunComponent>();
	}

	// Get and count all Pick Up on the Map
	TArray<AActor*> PickUpArray;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), PickUpActorTag, PickUpArray);
	for (AActor* PickUp : PickUpArray)
	{
		if (UPickUpComponent* PickUpComponent = PickUp->FindComponentByClass<UPickUpComponent>())
		{
			// Count
			EPickUpType InPickUpType = PickUpComponent->GetPickUpType();
			PickUpSpawner[(uint8)InPickUpType].CurrentPickUpOfThisType++;
			CurrentPickUpOnMap++;

			// Bind to the destruction event
			PickUpComponent->OnPickUpDestroyedWithType.AddUniqueDynamic(this, &USpawnerComponent::OnPickUpDestroyed);
		}
	}
}

void USpawnerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Clear Timer
	ClearTimer();

	Super::EndPlay(EndPlayReason);
}

void USpawnerComponent::PreSave(FObjectPreSaveContext SaveContext)
{
	// Print a log if a Pick Up actor isn't linked
	for (FPickUpSpawnerStruct& SpawnerStruct : PickUpSpawner)
	{
		if (!SpawnerStruct.PickUpActor)
		{
			const FString PickUpName = UEnum::GetValueAsString(SpawnerStruct.PickUpType);
			UE_LOG(LogTemp, Warning, TEXT("WAIT ! You need to specify an Actor BP for %s in the Spawner Struct or you won't be able to spawn one !"), *PickUpName);
		}
	}

	Super::PreSave(SaveContext);
}

void USpawnerComponent::OnPickUpDestroyed(EPickUpType InPickUpType)
{
	PickUpSpawner[(uint8)InPickUpType].CurrentPickUpOfThisType--;
	CurrentPickUpOnMap--;
}

void USpawnerComponent::OnSpawnerTimerEnd()
{
	bCanSpawnPickUp = true;
	ClearTimer();
}

void USpawnerComponent::ClearTimer()
{
	// Clear timer
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.ClearTimer(SpawnerTimerHandle);
}

void USpawnerComponent::OnPickUpInfoInputPressed()
{
	UE_LOG(LogTemp, Log, TEXT("-----------------------------------"));
	UE_LOG(LogTemp, Log, TEXT("There is a total of %d Pick Up on the map - You have a maximum capacity of %d"), CurrentPickUpOnMap, MaxPickUpOnMap);
	UE_LOG(LogTemp, Log, TEXT("There is %d Normal Pick Up"), PickUpSpawner[(uint8)EPickUpType::Normal].CurrentPickUpOfThisType);
	UE_LOG(LogTemp, Log, TEXT("There is %d Destroy After Pick Up"), PickUpSpawner[(uint8)EPickUpType::DestroyAfterPickUp].CurrentPickUpOfThisType);
	UE_LOG(LogTemp, Log, TEXT("There is %d Destroy After Throw"), PickUpSpawner[(uint8)EPickUpType::DestroyAfterThrow].CurrentPickUpOfThisType);
	UE_LOG(LogTemp, Log, TEXT("-----------------------------------"));
}

void USpawnerComponent::OnSpawnPickUpInputPressed(EPickUpType InPickUpType)
{
	// Can we spawn it ?
	if (!bCanSpawnPickUp)
	{
		UE_LOG(LogTemp, Log, TEXT("You need to wait, you can't spawn a Pick Up yet."));
		return;
	}

	// Is there too much Pick Up ?
	if (CurrentPickUpOnMap >= MaxPickUpOnMap)
	{
		UE_LOG(LogTemp, Log, TEXT("You can't spawn a Pick Up, there's already %d Pick Up on the map !"), CurrentPickUpOnMap);
		return;
	}

	const FString PickUpName = UEnum::GetValueAsString(InPickUpType);

	// Is our Pick Up Actor valid ?
	if (!PickUpSpawner[(uint8)InPickUpType].PickUpActor)
	{
		UE_LOG(LogTemp, Warning, TEXT("There is nothing to spawn for %s, check your Spawner Component !"), *PickUpName);
		return;
	}

	// Is our quota reached ?
	if(PickUpSpawner[(uint8)InPickUpType].CurrentPickUpOfThisType >= PickUpSpawner[(uint8)InPickUpType].MaxPickUpOfThisType)
	{
		UE_LOG(LogTemp, Warning, TEXT("You can't spawn %s, you have too many Pick Up of this type !"), *PickUpName);
		return;
	}

	// Spawn Pick Up
	AActor* NewPickUp = GetWorld()->SpawnActor(PickUpSpawner[(uint8)InPickUpType].PickUpActor);
	if (!NewPickUp)
	{
		UE_LOG(LogTemp, Error, TEXT("Something went wrong ! The %s Pick Up wasn't properly spawned."), *PickUpName);
		return;
	}

	// Bind to the destruction event
	UPickUpComponent* PickUpComponent = NewPickUp->FindComponentByClass<UPickUpComponent>();
	PickUpComponent->OnPickUpDestroyedWithType.AddUniqueDynamic(this, &USpawnerComponent::OnPickUpDestroyed);

	// Count the New Pick Up
	CurrentPickUpOnMap++;
	PickUpSpawner[(uint8)InPickUpType].CurrentPickUpOfThisType++;

	// Start Timer
	bCanSpawnPickUp = false;
	ClearTimer();
	FTimerManager& TimerManager = GetWorld()->GetTimerManager();
	TimerManager.SetTimer(SpawnerTimerHandle, this, &USpawnerComponent::OnSpawnerTimerEnd, TimeBetweenTwoSpawn, false);

	// Place the Pick Up on the Map
	if (GravityGunComponent.IsValid() && !GravityGunComponent->HasPickUpInHands())
	{
		GravityGunComponent->PlacePickUpInHands(NewPickUp);
	}
	else if (PlayerCameraManager.IsValid())
	{
		const FVector PickUpLocation = PlayerCameraManager->GetCameraLocation() + (PlayerCameraManager->GetActorForwardVector() * PickUpSpawnDistanceFromPlayer);
		const FQuat PickUpRotation = PlayerCameraManager->GetActorQuat();
		NewPickUp->SetActorLocationAndRotation(PickUpLocation, PickUpRotation);
	}
}