// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GravityGunComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FNumberPickUpTakenDelegate, int, PickUpTaken);


UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5INTRODUCTION_API UGravityGunComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGravityGunComponent();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	void OnTakeObjectInputPressed();
	void OnThrowObjectInputPressed();
	void OnThrowObjectInputReleased();

protected:
	TWeakObjectPtr<class AMainPlayer> Character = nullptr;
	TWeakObjectPtr<class APlayerCameraManager> CameraManager = nullptr;


// Collision
protected:
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Collision")
	TEnumAsByte<ETraceTypeQuery> GravityGunCollisionTraceChannel;
	ECollisionChannel GravityGunCollisionChannel;

	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Collision", meta = (ClampMin = "100.0", ClampMax = "1000.0"))
	float RaycastSize = 500.f;

// End of Collision


// Pick Up
protected:
	TWeakObjectPtr<AActor> CurrentPickUp = nullptr;
	TWeakObjectPtr<class UPickUpComponent> CurrentPickUpComponent = nullptr;
	TWeakObjectPtr<class UStaticMeshComponent> CurrentPickUpStaticMesh = nullptr;
	FName PreviousCollisionProfileName = NAME_None;

	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Pick Up", meta = (ClampMin = "100.0", ClampMax = "1000.0"))
	float PickUpDistanceFromPlayer = 200.f;
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Pick Up", meta = (ClampMin = "-200.0", ClampMax = "200.0"))
	float PickUpHeightFromPlayer = -10.f;
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Pick Up", meta = (ClampMin = "100.0", ClampMax = "10000.0"))
	float PickUpThrowForce = 5000.f;
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Pick Up")
	FVector PickUpAngularForce = FVector(50000.f, 30000.f, 60000.f);

protected:
	void UpdatePickUpLocation();
	void ReleasePickUp(bool bThrowPickUp = false);

	UFUNCTION()
	void OnHoldPickUpDestroyed();

// End of Pick Up


// Exercice 1
protected:
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Collision|Exercice 1", meta = (ClampMin = "100.0", ClampMax = "1000.0"))
	float RaycastSizeMin = 100.f;
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Collision|Exercice 1", meta = (ClampMin = "100.0", ClampMax = "1000.0"))
	float RaycastSizeMax = 1000.f;
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Collision|Exercice 1", meta = (ClampMin = "100.0", ClampMax = "1000.0", ToolTip = "Size that will be changed on the raycast each time you increase or decrease it."))
	float RaycastSizeUpdate = 100.f;


public:
	void OnIncreaseRaycastSize();
	void OnDecreaseRaycastSize();

// End of Exercice 1


// Exercice 2
protected:
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Pick Up", meta = (ClampMin = "100.0", ClampMax = "10000.0"))
	float PickUpMaxThrowForce = 10000.f;
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Pick Up", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float TimeToReachMaxThrowForce = 3.f;
	float CurrentTimeToReachMaxThrowForce = 0.f;
	
	bool bUpdateThrowForceTimer = false;

	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Pick Up", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float PickUpThrowForceMultiplier = 3.f;
	float CurrentPickUpThrowForceMultiplier = 1.f;

protected:
	void UpdateThrowForceTimer(float DeltaTime);

public:
	void OnThrowForceMultiplierInputPressed();
	
// End of Exercice 2


// Event for blueprints
protected:
	int PickUpTaken = 0;

	UPROPERTY(BlueprintAssignable, BlueprintCallable)
	FNumberPickUpTakenDelegate OnPickUpTaken;

// End of Event for blueprints


// UI
public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GravityGun")
	float GetTimeToReachMaxThrowForce();
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "GravityGun")
	float GetCurrentTimeToReachMaxThrowForce();

// End of UI


// Noted Exercice
public:
	bool HasPickUpInHands() const;
	void PlacePickUpInHands(AActor* InPickUp);
	void DestroyPickUpInHands();

// End of Noted Exercice


// Curves and Data Assets
protected:
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Curves")
	class UCurveFloat* ThrowForceCurve = nullptr;
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Data Asset")
	class UGravityGunDataAsset* GravityGunDataAsset = nullptr;

protected:
	void OnUpdateGravityGunDataAsset();

public:
#if WITH_EDITOR
	void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

// End of curvesand Data Assets


// Debug
protected:
	UPROPERTY(EditAnywhere, Category = "GravityGun|Debug")
	bool bDrawDebugRaycast = false;
	UPROPERTY(EditAnywhere, Category = "GravityGun|Debug", meta = (EditCondition = "bDrawDebugRaycast", EditConditionHides, ClampMin = "0.5", ClampMax = "10.0"))
	float TimerDebugRaycast = 5.f;

// End of Debug
};
