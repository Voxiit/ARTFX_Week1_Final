// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GravityGunController.generated.h"


UCLASS(Abstract, Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UE5INTRODUCTION_API UGravityGunController : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGravityGunController();

protected:
	virtual void BeginPlay() override;
		
protected:
	TWeakObjectPtr<class AMainPlayer> Character = nullptr;
	TWeakObjectPtr<class UGravityGunComponent> GravityGunComponent = nullptr;

// Inputs
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	class UInputAction* InputActionTakeObject;
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input")
	class UInputAction* InputActionThrowObject;

public:
	void SetupInputComponentGravityGun(TObjectPtr<class UInputComponent> InputComponent, class AMainPlayer* InCharacter);

protected:
	void OnTakeObjectInputPressed();
	void OnThrowObjectInputTriggered(const struct FInputActionValue& Value);

// End of Inputs


// Exercice 1
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input|Exercice 1")
	class UInputAction* InputActionRaycastSize;

protected:
	void OnUpdateRaycastSizeInputTriggered(const struct FInputActionValue& Value);

// End of Exercice 1


// Exercice 2
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Enhanced Input|Exercice 1")
	class UInputAction* InputActionThrowForceMultiplier;

protected:
	void OnThrowForceMultiplierInputPressed();

// End of Exercice 2
};
