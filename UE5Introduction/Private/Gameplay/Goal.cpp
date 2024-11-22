// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/Goal.h"
#include "Components/BoxComponent.h"
#include "Gameplay/PickUpComponent.h"
#include "Kismet/KismetSystemLibrary.h"

#include "Components/PointLightComponent.h"

// Sets default values
AGoal::AGoal(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;

	CollisionBox = ObjectInitializer.CreateDefaultSubobject<UBoxComponent>(this, "CollisionBox");
	if (CollisionBox)
	{
		SetRootComponent(CollisionBox);
	}
	PointLightComponent = ObjectInitializer.CreateDefaultSubobject<UPointLightComponent>(this, "GoalPointLight");
	if (PointLightComponent)
	{
		PointLightComponent->SetupAttachment(RootComponent);
	}
}

// Called when the game starts or when spawned
void AGoal::BeginPlay()
{
	Super::BeginPlay();

	// Bind to the overlap event
	if (CollisionBox)
	{
		CollisionBox->OnComponentBeginOverlap.AddUniqueDynamic(this, &AGoal::OnOverlapBegin);
	}
	else 
	{
		UE_LOG(LogTemp, Error, TEXT("THERE IS NO COLLISION BOX IN THE GOAL"));
	}
}

void AGoal::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UPickUpComponent* EnteringPickUp = OtherActor->FindComponentByClass<UPickUpComponent>();
	if (!EnteringPickUp)
	{
		UE_LOG(LogTemp, Warning, TEXT("THIS IS NOT A PICK UP"));
		return;
	}

	Score++;

	FString GoalName = UKismetSystemLibrary::GetDisplayName(this);
	OnSendScore.Broadcast(Score, GoalName);
	OnSendTeamScore.Broadcast(GoalTeam);
}

unsigned int AGoal::CountPickUpInside()
{
	// Check collision box
	if (!CollisionBox)
		return 0;

	// Prepare variables
	const FVector GoalLocation = GetActorLocation();
	const FVector BoxSize = CollisionBox->GetScaledBoxExtent();
	const FRotator GoalOrientation = GetActorRotation();
	const TArray<AActor*> ActorsToIgnore;
	TArray<FHitResult> HitResults;


	UKismetSystemLibrary::BoxTraceMulti(GetWorld(), GoalLocation, GoalLocation, BoxSize, GoalOrientation, 
		GoalCollisionTraceChannel,false, ActorsToIgnore, EDrawDebugTrace::None, HitResults, true);
	
	return HitResults.Num();
}

void AGoal::DisplayScore() const
{
	FString GoalName = UKismetSystemLibrary::GetDisplayName(this);
	UE_LOG(LogTemp, Log, TEXT("THE GOAL %s HAS A SCORE OF %d"), *GoalName, Score);
}

void AGoal::UpdatePointLight()
{
	if (!PointLightComponent)
		return;

	// Get Light Color
	FLinearColor NewLightColor;
	switch (GoalTeam)
	{
	case EGoalTeam::Red:
		NewLightColor = FLinearColor::Red;
		break;
	
	case EGoalTeam::Blue:
		NewLightColor = FLinearColor::Blue;
		break;
	
	case EGoalTeam::Yellow:
		NewLightColor = FLinearColor::Yellow;
		break;
	
	case EGoalTeam::Green:
		NewLightColor = FLinearColor::Green;
		break;
	
	default:
		NewLightColor = FLinearColor::White;
		break;
	}
	PointLightComponent->SetLightColor(NewLightColor);
}

#if WITH_EDITOR
void AGoal::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	
	// Update Goal Light
	if (!IsTemplate() && !HasAnyFlags(RF_NeedLoad))
	{
		UpdatePointLight();
	}
}
#endif