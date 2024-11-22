// Fill out your copyright notice in the Description page of Project Settings.


#include "Gameplay/GravityGunComponent.h"
#include "Player/MainPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Gameplay/PickUpComponent.h"

#include "Curves/CurveFloat.h"

#include "Components/StaticMeshComponent.h"
#include "Gameplay/GravityGunDataAsset.h"

UGravityGunComponent::UGravityGunComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}


void UGravityGunComponent::BeginPlay()
{
	Super::BeginPlay();

	Character = Cast<AMainPlayer>(GetOwner());
	CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	GravityGunCollisionChannel = UEngineTypes::ConvertToCollisionChannel(GravityGunCollisionTraceChannel);
}


void UGravityGunComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UpdatePickUpLocation();
	UpdateThrowForceTimer(DeltaTime);
}

void UGravityGunComponent::OnTakeObjectInputPressed()
{
	// Make sure that we don't already have a pick up in our hands
	if (CurrentPickUp.IsValid())
	{
		ReleasePickUp();
		return;
	}

	// First - Launch a raycast to find a pick up
	const FVector RaycastStart = CameraManager->GetCameraLocation();
	const FVector RaycastEnd = RaycastStart + (CameraManager->GetActorForwardVector() * RaycastSize);

	// Second - Prepare raycast params
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character.Get());

	// Third - Launch the raycast
#if !UE_BUILD_SHIPPING
	if(bDrawDebugRaycast)
		DrawDebugLine(GetWorld(), RaycastStart, RaycastEnd, FColor::Red, false, TimerDebugRaycast, 0, 3.5f);
#endif

	const bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, RaycastStart, RaycastEnd, GravityGunCollisionChannel, Params);
	if (!bHit)
	{
		UE_LOG(LogTemp, Log, TEXT("I HIT NOTHING"));
		return;
	}

	// Since we can now spawn a Pick Up, the second part of this function has been moved to a separate one
	PlacePickUpInHands(HitResult.GetActor());
}

void UGravityGunComponent::OnThrowObjectInputPressed()
{
	if (!CurrentPickUp.IsValid())
		return;

	// Exercice 2 - Prepare throw force timer
	CurrentTimeToReachMaxThrowForce = 0.f;
	bUpdateThrowForceTimer = true;
}

void UGravityGunComponent::OnThrowObjectInputReleased()
{
	if (!CurrentPickUp.IsValid())
		return;

	ReleasePickUp(true);

	// Exercice 2 - Reset throw force timer
	bUpdateThrowForceTimer = false;
	CurrentTimeToReachMaxThrowForce = 0.f;
}

void UGravityGunComponent::UpdatePickUpLocation()
{
	// Make sure we have something to update
	if (!CurrentPickUp.IsValid())
		return;

	// Compute its new location
	const FVector NewPickUpLocation = CameraManager->GetCameraLocation() + (CameraManager->GetActorForwardVector() * PickUpDistanceFromPlayer) + (CameraManager->GetActorUpVector() * PickUpHeightFromPlayer);
	const FRotator NewPickUpRotation = CameraManager->GetCameraRotation();
	CurrentPickUp->SetActorLocationAndRotation(NewPickUpLocation, NewPickUpRotation);
}

void UGravityGunComponent::ReleasePickUp(bool bThrowPickUp)
{
	// Unbind event if necessary
	if (CurrentPickUpComponent->GetPickUpType() == EPickUpType::DestroyAfterPickUp)
	{
		CurrentPickUpComponent->OnPickUpDestroyed.RemoveDynamic(this, &UGravityGunComponent::OnHoldPickUpDestroyed);
	}

	// Set back the physic
	CurrentPickUpStaticMesh->SetSimulatePhysics(true);

	// Set back the coll profile
	CurrentPickUpStaticMesh->SetCollisionProfileName(PreviousCollisionProfileName);

	// Throw Pick Up
	if (bThrowPickUp)
	{
		float ThrowForceAlpha;
		float ThrowForce;
		// Use the Data Asset
		if (GravityGunDataAsset)
		{
			ThrowForceAlpha = FMath::Clamp(CurrentTimeToReachMaxThrowForce / GravityGunDataAsset->TimeToReachMaxThrowForce, 0.f, 1.f);
			ThrowForce = FMath::Lerp(GravityGunDataAsset->PickUpThrowForce, GravityGunDataAsset->PickUpMaxThrowForce, ThrowForceAlpha) * CurrentPickUpThrowForceMultiplier;
		}
		// Use a curve if ther's one, if there's nothing, stick with the Throw Force Alpha
		else if (ThrowForceCurve)
		{
			ThrowForce = ThrowForceCurve->GetFloatValue(CurrentTimeToReachMaxThrowForce) * CurrentPickUpThrowForceMultiplier;
		}
		// Normal Way
		else
		{
			ThrowForceAlpha = FMath::Clamp(CurrentTimeToReachMaxThrowForce / TimeToReachMaxThrowForce, 0.f, 1.f);
			ThrowForce = FMath::Lerp(PickUpThrowForce, PickUpMaxThrowForce, ThrowForceAlpha) * CurrentPickUpThrowForceMultiplier;
		}
		

		UE_LOG(LogTemp, Log, TEXT("MY THROW FORCE IS %f - THE TIMER WAS %f"), ThrowForce, CurrentTimeToReachMaxThrowForce);

		const FVector Impusle = CameraManager->GetActorForwardVector() * ThrowForce;
		CurrentPickUpStaticMesh->AddImpulse(Impusle);

		FVector AngularImpulse;
		// Check for data asset
		if (GravityGunDataAsset)
		{
			AngularImpulse = FVector(FMath::RandRange(.0, GravityGunDataAsset->PickUpAngularForce.X), FMath::RandRange(.0, GravityGunDataAsset->PickUpAngularForce.Y), FMath::RandRange(.0, GravityGunDataAsset->PickUpAngularForce.Z));
		}
		// If there's none, use the normal way
		else
		{
			AngularImpulse = FVector(FMath::RandRange(.0, PickUpAngularForce.X), FMath::RandRange(.0, PickUpAngularForce.Y), FMath::RandRange(.0, PickUpAngularForce.Z));
		} 

		CurrentPickUpStaticMesh->AddAngularImpulseInDegrees(AngularImpulse);
	
		// Check if destruction is required
		if (CurrentPickUpComponent->GetPickUpType() == EPickUpType::DestroyAfterThrow)
			CurrentPickUpComponent->StartPickUpDestructionTimer();
	}

	// Clean pointers
	CurrentPickUp = nullptr;
	CurrentPickUpComponent = nullptr;
	CurrentPickUpStaticMesh = nullptr;
}

void UGravityGunComponent::OnHoldPickUpDestroyed()
{
	ReleasePickUp();
}

void UGravityGunComponent::OnIncreaseRaycastSize()
{
	RaycastSize = FMath::Clamp(RaycastSize + RaycastSizeUpdate, RaycastSizeMin, RaycastSizeMax);
	UE_LOG(LogTemp, Log, TEXT("NEW RAYCAST SIZE IS %f"), RaycastSize);
}

void UGravityGunComponent::OnDecreaseRaycastSize()
{
	RaycastSize = FMath::Clamp(RaycastSize - RaycastSizeUpdate, RaycastSizeMin, RaycastSizeMax);
	UE_LOG(LogTemp, Log, TEXT("NEW RAYCAST SIZE IS %f"), RaycastSize);
}

void UGravityGunComponent::UpdateThrowForceTimer(float DeltaTime)
{
	if (!bUpdateThrowForceTimer)
		return;

	CurrentTimeToReachMaxThrowForce += DeltaTime;

	// This is a bonus - Predict projectile path
	/*
	FPredictProjectilePathParams PredictParams;
	FPredictProjectilePathResult PredictResult;
	PredictParams.StartLocation = CurrentPickUp->GetActorLocation();
	PredictParams.LaunchVelocity = CameraManager->GetActorForwardVector() * PickUpThrowForce;
	PredictParams.bTraceWithCollision = true;
	PredictParams.ProjectileRadius = 0.f;
	PredictParams.MaxSimTime = 2.f;
	PredictParams.bTraceWithChannel = true;
	PredictParams.TraceChannel = ECollisionChannel::ECC_WorldStatic;
	PredictParams.ObjectTypes.Add(EObjectTypeQuery::ObjectTypeQuery1);
	PredictParams.SimFrequency = 25.f;
	PredictParams.DrawDebugType = EDrawDebugTrace::ForDuration;
	PredictParams.DrawDebugTime = 5.f;
	PredictParams.bTraceComplex = false;
	UGameplayStatics::PredictProjectilePath(GetWorld(), PredictParams, PredictResult);
	*/
}

void UGravityGunComponent::OnThrowForceMultiplierInputPressed()
{
	CurrentPickUpThrowForceMultiplier = CurrentPickUpThrowForceMultiplier == 1.f ? PickUpThrowForceMultiplier : 1.f;
}

float UGravityGunComponent::GetTimeToReachMaxThrowForce()
{
	return GravityGunDataAsset ? GravityGunDataAsset->TimeToReachMaxThrowForce : TimeToReachMaxThrowForce;
}

float UGravityGunComponent::GetCurrentTimeToReachMaxThrowForce()
{
	return CurrentTimeToReachMaxThrowForce;
}

bool UGravityGunComponent::HasPickUpInHands() const
{
	return IsValid(CurrentPickUp.Get());
}

void UGravityGunComponent::PlacePickUpInHands(AActor* InPickUp)
{
	// Check for Pick Up Component
	CurrentPickUpComponent = InPickUp->GetComponentByClass<UPickUpComponent>();
	if (!CurrentPickUpComponent.IsValid())
		return;

	// Check for Mesh component
	CurrentPickUpStaticMesh = InPickUp->GetComponentByClass<UStaticMeshComponent>();
	if (!CurrentPickUpStaticMesh.IsValid())
		return;

	// Get its pointer
	CurrentPickUp = InPickUp;

	// Print Name
	FString ActorName = UKismetSystemLibrary::GetDisplayName(InPickUp);
	FString UnrealActorName = InPickUp->GetName();
	UE_LOG(LogTemp, Log, TEXT("I HIT SOMETHING - MY NAME %s - UNREAL NAME %s"), *ActorName, *UnrealActorName);

	// Disable the physics
	CurrentPickUpStaticMesh->SetSimulatePhysics(false);

	// Update the collision profile
	PreviousCollisionProfileName = CurrentPickUpStaticMesh->GetCollisionProfileName();
	CurrentPickUpStaticMesh->SetCollisionProfileName(UCollisionProfile::NoCollision_ProfileName);

	// Check if destruction is required
	switch (CurrentPickUpComponent->GetPickUpType())
	{
	case EPickUpType::DestroyAfterPickUp:
		// Launch the timer
		CurrentPickUpComponent->StartPickUpDestructionTimer();

		// Bind our callback on the event
		CurrentPickUpComponent->OnPickUpDestroyed.AddUniqueDynamic(this, &UGravityGunComponent::OnHoldPickUpDestroyed);
		break;

	case EPickUpType::DestroyAfterThrow:
		// Clear the timer so it doesn't dissapear in our hands
		CurrentPickUpComponent->ClearDestructionTimer();
		break;

	default:
		break;
	}

	// Send event to blueprint
	PickUpTaken++;
	OnPickUpTaken.Broadcast(PickUpTaken);
}

void UGravityGunComponent::DestroyPickUpInHands()
{
	if (!HasPickUpInHands())
		return;

	UPickUpComponent* PickUpComponentToDestroy = CurrentPickUpComponent.Get();
	ReleasePickUp();
	PickUpComponentToDestroy->DestroyPickUp();
}

void UGravityGunComponent::OnUpdateGravityGunDataAsset()
{
	if (!GravityGunDataAsset)
		return;

	TimeToReachMaxThrowForce = GravityGunDataAsset->TimeToReachMaxThrowForce;
}

#if WITH_EDITOR
void UGravityGunComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Get what was changed
	FName PropertyName = (PropertyChangedEvent.Property != NULL) ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	// We only update the blueprint
	if (IsTemplate() && !HasAnyFlags(RF_NeedLoad))
	{
		if (PropertyName == GET_MEMBER_NAME_CHECKED(UGravityGunComponent, GravityGunDataAsset))
		{
			OnUpdateGravityGunDataAsset();
		}
	}
}
#endif