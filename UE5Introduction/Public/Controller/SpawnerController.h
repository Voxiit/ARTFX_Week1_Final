// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpawnerController.generated.h"


UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5INTRODUCTION_API USpawnerController : public UActorComponent
{
	GENERATED_BODY()

public:	
	USpawnerController();

	void SetupInputPlayerController(TObjectPtr<class UInputComponent> InputComponent, class AMainPlayer* InCharacter);

protected:
	TWeakObjectPtr<class AMainPlayer> Character = nullptr;
	TWeakObjectPtr<class USpawnerComponent> SpawnerComponent = nullptr;
	TWeakObjectPtr<class UGravityGunComponent> GravityGunComponent = nullptr;

// Inputs
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	class UInputAction* InputActionSpawnPickUp;
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	class UInputAction* InputActionPickUpInfo;
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	class UInputAction* InputActionDestroyPickUp;

protected:
	void OnSpawnPickUpInputPressed(const struct FInputActionValue& Value);
	void OnPickUpInfoInputPressed();
	void OnDestroyPickUpPressed();

// End of Inputs
		
};
