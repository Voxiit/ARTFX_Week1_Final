// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Gameplay/MyEnums.h"
#include "ScoreComponent.generated.h"


USTRUCT(BlueprintType)
struct FTeamStruct
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Goal Team")
	EGoalTeam GoalTeam = EGoalTeam::None;
	UPROPERTY(VisibleAnywhere, Category = "Goal Team")
	unsigned int TeamScore = 0;
};

UCLASS(Abstract, Blueprintable, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class UE5INTRODUCTION_API UScoreComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UScoreComponent();

protected:
	virtual void BeginPlay() override;


// Exercice 3
protected:
	TArray<AActor*> GoalArray;

protected:
	UFUNCTION()
	void OnSendScore(unsigned int Score, FString GoalName);

public:
	void OnDisplayScoreInputPressed();

// End of Exercice 3


// Exercice 4
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Score")
	TArray<FTeamStruct> TeamArray;
	TStaticArray<FTeamStruct, (uint8)EGoalTeam::MAX> TeamStaticArray;

protected:
	UFUNCTION()
	void OnSendTeamScore(enum EGoalTeam GoalTeam);

// End of Exercice 4
};
