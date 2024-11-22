// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

static FName PickUpActorTag = "PICKUP";

UENUM()
enum class EGoalTeam : uint8
{
	None UMETA(Hidden),
	Red,
	Blue,
	Yellow,
	Green,
	MAX UMETA(Hidden),
};

UENUM()
enum class EPickUpType : uint8
{
	Normal,
	DestroyAfterPickUp,
	DestroyAfterThrow,
	MAX UMETA(Hidden),
};
