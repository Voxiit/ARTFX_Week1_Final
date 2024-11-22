// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GravityGunDataAsset.generated.h"


UCLASS()
class UE5INTRODUCTION_API UGravityGunDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Pick Up", meta = (ClampMin = "100.0", ClampMax = "10000.0"))
	float PickUpThrowForce = 5000.f;
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Pick Up", meta = (ClampMin = "100.0", ClampMax = "10000.0"))
	float PickUpMaxThrowForce = 10000.f;
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Pick Up")
	FVector PickUpAngularForce = FVector(50000.f, 30000.f, 60000.f);
	UPROPERTY(EditDefaultsOnly, Category = "GravityGun|Pick Up", meta = (ClampMin = "0.1", ClampMax = "10.0"))
	float TimeToReachMaxThrowForce = 3.f;
};
