// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/ScoreComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"

#include "Gameplay/Goal.h"

UScoreComponent::UScoreComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}


void UScoreComponent::BeginPlay()
{
	Super::BeginPlay();
	
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGoal::StaticClass(), GoalArray);
	for (AActor* Goal : GoalArray)
	{
		AGoal* GoalCasted = Cast<AGoal>(Goal);
		if (GoalCasted)
		{
			// Exercice 3
			GoalCasted->OnSendScore.AddUniqueDynamic(this, &UScoreComponent::OnSendScore);
		
			// Exercice 4
			GoalCasted->OnSendTeamScore.AddUniqueDynamic(this, &UScoreComponent::OnSendTeamScore);
		}
	}

	// Exercice 4 - Quickly initialize our array
	for (const FTeamStruct& TeamStruct : TeamArray)
		TeamStaticArray[(uint8)TeamStruct.GoalTeam] = TeamStruct;
}

void UScoreComponent::OnSendScore(unsigned int Score, FString GoalName)
{
	UE_LOG(LogTemp, Log, TEXT("THE GOAL %s HAS A SCORE OF %d"), *GoalName, Score);
}

void UScoreComponent::OnDisplayScoreInputPressed()
{
	// Exercice 3
	for (AActor* Goal : GoalArray)
	{
		AGoal* GoalCasted = Cast<AGoal>(Goal);
		if (GoalCasted)
		{
			//GoalCasted->DisplayScore();

			unsigned int PickUpInside = GoalCasted->CountPickUpInside();
			FString GoalName = UKismetSystemLibrary::GetDisplayName(GoalCasted);
			//UE_LOG(LogTemp, Log, TEXT("THE GOAL %s HAS CURRENTLY %d PICK UP INSIDE"), *GoalName, PickUpInside);
		}
	}

	// Exercice 4
	for (const FTeamStruct& TeamStruct : TeamStaticArray)
	{
		if (TeamStruct.GoalTeam != EGoalTeam::None)
		{
			FString EnumName;
			UEnum::GetValueAsString(TeamStruct.GoalTeam, EnumName);
			UE_LOG(LogTemp, Log, TEXT("%s TEAM HAS A SCORE OF %d"), *EnumName, TeamStruct.TeamScore);
		}
	}
}

void UScoreComponent::OnSendTeamScore(EGoalTeam GoalTeam)
{
	TeamStaticArray[(uint8)GoalTeam].TeamScore++;
}