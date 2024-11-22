// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyEnums.h"
#include "SpawnerComponent.generated.h"

USTRUCT(BlueprintType)
struct FPickUpSpawnerStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "PickUp")
	TSubclassOf<AActor> PickUpActor = nullptr;
	UPROPERTY(EditAnywhere, Category = "PickUp")
	EPickUpType PickUpType = EPickUpType::Normal;

	UPROPERTY(EditAnywhere, Category = "PickUp", meta = (ClampMin = "1.0", ClampMax = "15"))
	int MaxPickUpOfThisType = 10;
	int CurrentPickUpOfThisType = 0;
};

UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5INTRODUCTION_API USpawnerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpawnerComponent();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	virtual void PreSave(FObjectPreSaveContext SaveContext) override;


protected:
	TWeakObjectPtr<class AMainPlayer> Character = nullptr;
	TWeakObjectPtr<class UGravityGunComponent> GravityGunComponent = nullptr;
	TWeakObjectPtr<class APlayerCameraManager> PlayerCameraManager = nullptr;

// Pick Up
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Spawner", meta = (ClampMin = "5", ClampMax = "50"))
	int MaxPickUpOnMap = 30;
	int CurrentPickUpOnMap = 0; // In order to count the Pick Up already on the map, I'm going to find them with a Tag

	UPROPERTY(EditDefaultsOnly, Category = "Spawner")
	FPickUpSpawnerStruct PickUpSpawner[(uint8)EPickUpType::MAX];

protected:
	UFUNCTION()
	void OnPickUpDestroyed(EPickUpType InPickUpType);

// End of Pick Up


// Spawn Pick Up
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Spawner", meta = (ClampMin = "1", ClampMax = "10"))
	float TimeBetweenTwoSpawn = 2.f;
	FTimerHandle SpawnerTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "Spawner", meta = (ClampMin = "100", ClampMax = "500", ToolTip = "If the player already has a Pick Up in its hand, the new spawned Pick Up will be created in the player's camera forward direction at this distance."))
	float PickUpSpawnDistanceFromPlayer = 200.f;

	bool bCanSpawnPickUp = true;

protected:
	void OnSpawnerTimerEnd();
	void ClearTimer();

// End of Spawn Pick Up


// Inputs
public:
	void OnPickUpInfoInputPressed();
	void OnSpawnPickUpInputPressed(EPickUpType InPickUpType);

// End of Inputs	
};
