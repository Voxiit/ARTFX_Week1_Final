// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Gameplay/MyEnums.h"
#include "Goal.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FSendScoreDelegate, unsigned int, Score, FString, GoalName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSendTeamScoreDelegate, EGoalTeam, GoalTeam);


UCLASS()
class UE5INTRODUCTION_API AGoal : public AActor
{
	GENERATED_BODY()
	
public:	
	AGoal(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;

// Collision Box
protected:
	UPROPERTY(EditDefaultsOnly)
	class UBoxComponent* CollisionBox = nullptr;
	unsigned int Score = 0;

	UPROPERTY(EditDefaultsOnly, Category = "Goal")
	TEnumAsByte<ETraceTypeQuery> GoalCollisionTraceChannel;
	
protected:
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	unsigned int CountPickUpInside();

// End of Collision Box


// Exercice 3
public:
	FSendScoreDelegate OnSendScore;

public:
	void DisplayScore() const;

// End of Exercice 3


// Exercice 4
protected:
	UPROPERTY(EditInstanceOnly, Category = "Goal")
	EGoalTeam GoalTeam = EGoalTeam::None;

public:
	FSendTeamScoreDelegate OnSendTeamScore;

// End of Exercice 4


// On Construction
protected:
	UPROPERTY(EditAnywhere)
	class UPointLightComponent* PointLightComponent = nullptr;

protected:
	void UpdatePointLight();

public:
#if WITH_EDITOR
	virtual void OnConstruction(const FTransform& Transform) override;
#endif

// End of On Construction
};
